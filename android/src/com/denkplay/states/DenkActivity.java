package com.denkplay.states;

import org.qtproject.qt.android.bindings.QtActivity;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.content.Intent;
import android.content.Context;
import android.widget.Toast;
import android.media.AudioManager;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import androidx.core.content.ContextCompat;
import android.bluetooth.BluetoothManager;

import android.content.ComponentName;
import android.content.ServiceConnection;
import com.denkplay.states.DenkService;

public class DenkActivity extends QtActivity {
    private Context context;
    private int myState_;

/* For holding the binded service BEGINS */
    private DenkService theService_;
    private boolean serviceStarted_ = false;
    private boolean serviceBinded_ = false;
/* For holding the binded service ENDS */

/* For holding vars required to manage audio interrupts BEGINS */
    // AudioAttributes describes to android the use case for your audio.
    // The system looks at them when an app gains and loses audio focus.
    private AudioAttributes audioAttributes_ = new AudioAttributes.Builder()
        .setUsage(AudioAttributes.USAGE_MEDIA)
        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
        .build();
    //
    private AudioManager.OnAudioFocusChangeListener audioFocusChangeListener_ = new AudioManager.OnAudioFocusChangeListener() {
        @Override
        public void onAudioFocusChange(int focusChange) {
            switch (focusChange) {
                case AudioManager.AUDIOFOCUS_LOSS:
                    // Permanent loss of audio focus. Pause playback indefinitely
                    theService_.controlCPPMediaControl("com.denkplay.states.action.pause");
                    break;
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
                    // Audio session Capturer says we should pause pausing
                    theService_.controlCPPMediaControl("com.denkplay.states.action.suspend");
                    break;
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                    // Audio session Capturer says we can lower the volume and keep playing rather than pausing
                    theService_.controlCPPMediaControl("com.denkplay.states.action.suspend");
                    break;
                case AudioManager.AUDIOFOCUS_GAIN:
                    // Your app has been re-granted audio focus again.
                    // This is called after the capturer who captured the audio
                    // session with either "AUDIOFOCUS_LOSS_TRANSIENT" or
                    // "AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK" finally releases the
                    // audio session. Now you can unsuspend your play.
                    theService_.controlCPPMediaControl("com.denkplay.states.action.unsuspend");
                    break;
            }
        }
    };
    // AudioFocusRequest is used to pass the AudioAttributes we built above to
    // Android, and also tell Android how we intend to seize the audiosession's focus.
    // This info enables android to notify the other app (if any) currently using audio session.
    // A param of "AudioManager.AUDIOFOCUS_GAIN" means we want full audio focus permanently.
    // "AUDIOFOCUS_GAIN_TRANSIENT" means we want full audio focus but for less than 45 seconds.
    // "AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK" but for less than 45 seconds, and in the meantime, the other app currently holding audio session may decide to duck(lower their volume) rather than pause.
    private AudioFocusRequest focusRequest_ = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
        .setAudioAttributes(audioAttributes_)
        .setAcceptsDelayedFocusGain(true)
        .setOnAudioFocusChangeListener(audioFocusChangeListener_)
        .build();
/* For holding vars required to manage audio interrupts ENDS */


/* [[1]] Needed for detecting when bluetooth connect/disconnect from something */
    private BroadcastReceiver bluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action == null) return;
            if (action.equals(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED)) {
                switch (intent.getIntExtra(BluetoothAdapter.EXTRA_CONNECTION_STATE, -1)) {
                    case BluetoothAdapter.STATE_DISCONNECTED:
                        theService_.controlCPPMediaControl("com.denkplay.states.action.pause");
                        break;
                    case BluetoothAdapter.STATE_CONNECTED:
                        // do nothing coz if you play/unsuspend, it will breach user privacy when
                        // BluetoothAdapter.STATE_DISCONNECTED paused the audio, then a phone call came in
                        // later and at the end of the phone call, the audio gets unsuspended into a loud speaker.
                        break;
                }
            }
        }
    };
/* [[1]] ENDS */


/* [[2]] Needed for detecting when wire headset connect/disconnect */
    private BroadcastReceiver wireHeadsetReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action == null) return;

            if (action.equals(Intent.ACTION_HEADSET_PLUG)) {
                switch (intent.getIntExtra("state", -1)) {
                    case 0:
                    /* if cond necessary to prevent app crash coz this callback is fired at app launch (maybe coz it used "registerReceiver" instead of "ContextCompat.registerReceiver"), at the time when theService_ is not yet started */
                        if (serviceStarted_)
                            theService_.controlCPPMediaControl("com.denkplay.states.action.pause");;
                        break;
                    case 1:
                        break;
                }
            }
        }
    };
/* [[2]] ENDS */


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = this;
        myState_ = 0;

    /* [[1]] Needed for detecting when bluetooth connect/disconnect from something */
    /*  NOW, SINCE WE HAVE BUILT OUR BroadcastReceiver CALLBACK AND STORED IT
    *   TO VARIABLE bluetoothReceiver, WE NOW NEED TO RUN THE FOLLOWING LINES
    *   ON APP LAUNCH TO RETRIEVE THE RUNNING INSTANCE OF BLUETOOTH SERVICE
    *   AND TELL IT TO CALL OUR BroadcastReceiver CALLBACK EACH TIME THE STATE
    *   OF CONNECTED BLUETOOTH DEVICE CHANGES
    */
        // First we retrieve the live instance of Android Bluetooth Service
        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(context.BLUETOOTH_SERVICE);
        // Then from there, we obtain the local device Bluetooth adapter
        BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();
        // Now, register listener for listening to changes in the local device Bluetooth adapter
        ContextCompat.registerReceiver(this, bluetoothReceiver, new IntentFilter(bluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED), ContextCompat.RECEIVER_EXPORTED);

    /* [[2]] Needed for detecting when wire headset connect/disconnect */
        IntentFilter filter = new IntentFilter(Intent.ACTION_HEADSET_PLUG);
        registerReceiver(wireHeadsetReceiver, filter);

        Toast.makeText(this, "Starting Dinkplay..", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onStart() {
        super.onStart();
        myState_ = 1;
    }

    @Override
    public void onResume() {
        super.onResume();
        myState_ = 2;
    }

    @Override
    protected void onPause() {
        super.onPause();
        myState_ = 3;
        // moveTaskToBack (true);
    }

    @Override
    protected void onStop() {
        super.onStop();
        myState_ = 4;
        // Log.d("Rachit", "In Stop Method");
        // Log.d("Rachit", Boolean.toString(serviceBinded_));
    }

    @Override
    public void onDestroy() {
        unbindService(connection);
        serviceBinded_ = false;

    /* [[1]] Needed for detecting when bluetooth connect/disconnect from something */
        // remove listener for listening to changes in the local device Bluetooth adapter
        unregisterReceiver(bluetoothReceiver);

    /* [[2]] Needed for detecting when wire headset connect/disconnect */
        unregisterReceiver(wireHeadsetReceiver);

        // This is a clean way to quickly exit the app onDestroy.
        // if you depend on super.onDestroy, exiting can take over
        // 20 seconds after app closes. Also, startedServices wont
        // get their ondestroy called at all, orphaning them in the
        // process. an
        // Worse is if you attempt to relaunch the app, it will hang
        // and crash.
        // The only way to avoid all these problem is to cleanly exit
        // using the below two calls.
        finishAffinity();
        System.exit(0);

        super.onDestroy();
    }

////////////////////////////////////////////////////////////////
// ******* METHODS FOR STARTING AND BINDING SERVICE BEGINS *****
////////////////////////////////////////////////////////////////
    @Override
    public ComponentName startForegroundService(Intent intent) {
        // only start this service if not yet started
        if (! serviceStarted_) {
            ComponentName componentName = super.startForegroundService(intent);
            serviceStarted_ = true;
        }

        // Bind to DenkService AKA the started foreground service
        bindService(intent, connection, 0);

        // cannot return componentName anymore due to our implementation
        return null;
    }

    /** connection is simply 2 callbacks. one used by bindService(), the other used by unbindService() */
    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to DenkService, cast the IBinder and get DenkService instance.
            DenkService.MyBinder binder = (DenkService.MyBinder) service;
            theService_ = binder.getService();
            serviceBinded_ = true;

        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            serviceBinded_ = false;
        }
    };
/////////////////////////////////////////////////////////
// **** METHODS FOR STARTING AND BINDING SERVICE ENDS ***
/////////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// **** METHODS DIRECTLY CALLED FROM CPP BEGINS ******
//////////////////////////////////////////////////////
    /*
    * We want to access the method inside service from
    * Cpp. This method is just to enable CPP reach the
    * setPlayPauseIconInService() method inside the
    * binded service instance theService_
    */
    public void setPlayPauseIconInActivity(boolean playing) {
        /* if cond necessary to prevent app crash coz this method is fired at app launch,
        for whatever reason. At the time when theService_ is not yet started */
        if (serviceStarted_)
            theService_.setPlayPauseIconInService(playing);
    }

    // Checked before playing. if failed, it returns false
    // to cpp. which will cause the play() in cpp to not exec
    public boolean seizeControlOfAudioManager() {
    /* NOW SINCE WE HAVE BUILT OUR LISTENERS AND STORED THE FINAL TO focusRequest_,
    // WE NOW NEED TO RUN THE BELOW EACH TIME WE NEED TO START .
    // WHAT THE BELOW CODE DOES IN THIS CASE IS THAT IT REQUEST ANDROID's audiosession
    // MANAGER TO GRANT US CONTROL OF AUDIO SESSION AND IT PASSES OUR focusRequest_
    // OBJECT(WHICH CONTAINS A DESCRIPTION OF HOW/WHY WE NEED THE audiosession) TO ANDROID.
    // AND SINCE OUR focusRequest_ OBJECT ALSO CONTAINS THE LISTENER FOR INTERRUPTIONS,
    // ANDROID WILL EXECUTE THAT LISTENER IF AN INTERRUPT COMES IN WHILE WE HAPPEN TO BE
    // IN CONTROL OF audiosession.
    */
        // retrieve the running instance of audiosession manager from android
        AudioManager audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        // request the audio focus from android and store response in the int variable to know android's response
        int result = audioManager.requestAudioFocus(focusRequest_);
        if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED) {
            return true;
        } else if (result == AudioManager.AUDIOFOCUS_REQUEST_FAILED) {
            return false;
        }
        return false;
    }

    /**
      * Called from frontend to decide what
      * should happen when user swipes up to
      * kill the app from minimized view, or
      * clicks back button to kill the app.
      * Both sends same onClose signal from
      * QML part of Qt.
      * @returns void
      */
    public void myBackPressManager() {
        if (myState_ == 2) {
            moveTaskToBack (true);
            super.onPause();
        } else {
            // Instant and clean exit. All running services onDestroy()
            // will be called as well to ensure startedServices also have
            // the opportunity to close themselves gracefully.
            // finishAffinity() kills app and frees memory.
            // System.exit(0); forces android to take back freed memory.
            finishAffinity();
            System.exit(0);
        }
    }

}

