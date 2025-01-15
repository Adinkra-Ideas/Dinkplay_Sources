package com.denkplay.states;

import android.content.Context;
import android.content.Intent;
import android.app.PendingIntent;
import android.util.Log;
import android.os.IBinder;
import android.os.Binder;
import android.os.Bundle;
import android.widget.Toast;
import android.app.ActivityManager;
// import android.content.ServiceConnection;
// import android.content.ComponentName;

import android.R;
// import android.os.Build;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import androidx.core.app.NotificationCompat;
import androidx.media.app.NotificationCompat.MediaStyle;
import android.graphics.BitmapFactory;

import org.qtproject.qt.android.bindings.QtService;

public class DenkService extends QtService
{
    private static native void sendToQt(String message);

    private Context context = null;
    private int ret_;

    public boolean playing_ = false;

    DenkService denkService;

    @Override
    public void onCreate() {
        super.onCreate();
        context = this;

        // Create the notif channel for our Notification way ahead of time
        final String ChannelID = "DinkPlay";
        NotificationChannel channel = new NotificationChannel(
                ChannelID,
                ChannelID,
                NotificationManager.IMPORTANCE_LOW
            );
        getSystemService(NotificationManager.class)
            .createNotificationChannel(channel);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Turn off the background service
        stopSelf();
    }


     // we will move the lines to a declarative function
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // Filter the intent and inform CPP if it was
        // a click from prev, play or pause
        String actionRcvd = intent.getAction();
        controlCPPMediaControl(actionRcvd);
        // if (actionRcvd != null) {
        //     if (actionRcvd.equals("com.denkplay.states.action.prev")) {
        //         sendToQt("((((Prev))))");
        //     } else if (actionRcvd.equals("com.denkplay.states.action.play")) {
        //         sendToQt("((((Play))))");
        //     } else if (actionRcvd.equals("com.denkplay.states.action.pause")) {
        //     sendToQt("((((Pause))))");
        //     } else if (actionRcvd.equals("com.denkplay.states.action.next")) {
        //         sendToQt("((((Next))))");
        //     }
        // }


        // Call the Activity AKA ThisApp when this foreground service is clicked
        // PendingIntent.getActivity simply pulls the activity AKA App into the
        // front of phone
        Intent activityIntent = new Intent(this, DenkActivity.class);
        PendingIntent pendingActIntent = PendingIntent.getActivity(context, 0, activityIntent, PendingIntent.FLAG_IMMUTABLE);

        // PendingIntent.getService Calls the Service.onStartCommand directly.

        // When the user
        Intent previousIntent = new Intent(this, DenkService.class);
        previousIntent.setAction("com.denkplay.states.action.prev");
        PendingIntent ppreviousIntent = PendingIntent.getForegroundService(this, 0, previousIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        Intent playIntent = new Intent(this, DenkService.class);
        playIntent.setAction("com.denkplay.states.action.play");
        PendingIntent pplayIntent = PendingIntent.getForegroundService(this, 0, playIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        Intent pauseIntent = new Intent(this, DenkService.class);
        pauseIntent.setAction("com.denkplay.states.action.pause");
        PendingIntent ppauseIntent = PendingIntent.getForegroundService(this, 0, pauseIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        Intent nextIntent = new Intent(this, DenkService.class);
        nextIntent.setAction("com.denkplay.states.action.next");
        PendingIntent pnextIntent = PendingIntent.getForegroundService(this, 0, nextIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        // Build the notification
        final String ChannelID = "DinkPlay";
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, ChannelID)
                .setStyle(new MediaStyle()
                               .setShowActionsInCompactView(0, 1, 2)
                        )
                .setSmallIcon(getApplicationInfo().icon)
                .setLargeIcon(BitmapFactory.decodeResource(getResources(), getApplicationInfo().icon))
                .setContentTitle("")
                .setContentText("")
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setContentIntent(pendingActIntent)
                .setOngoing(true)
                .addAction(new NotificationCompat.Action(R.drawable.ic_media_previous, "Previous", ppreviousIntent))
                .addAction(new NotificationCompat.Action(
                        (playing_ ? R.drawable.ic_media_pause : R.drawable.ic_media_play),
                        (playing_ ? "Pause" : "Play"),
                        (playing_ ? ppauseIntent : pplayIntent)))
                .addAction(new NotificationCompat.Action(R.drawable.ic_media_next, "Next", pnextIntent))
                ;

            // param3 of 2 == FOREGROUND_SERVICE_TYPE_MEDIA_PLAYBACK
            // as mandated by API >= 33
            startForeground(1002, builder.build(), 2);

            int ret = super.onStartCommand(intent, flags, startId);
            return ret;
    }

    // this need to be renamed to useDummyIntentToRefreshBGServiceNotif()
    public void setPlayPauseIconInService(boolean playing) {
        playing_ = playing;

        /* We will manually execute a pendingIntent so that the onStartCommand will be called for our music player icons to be refreshed */
        Intent refreshIconsIntent = new Intent(this, DenkService.class);
        refreshIconsIntent.setAction("com.denkplay.states.action.refreshIcons");
        PendingIntent prefreshIconsIntent = PendingIntent.getForegroundService(this, 0, refreshIconsIntent, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);

        // Execute the operation associated with our pendingIntent as if it were clicked by a user
        try {
            prefreshIconsIntent.send();
        } catch (PendingIntent.CanceledException e) {
            // e.printStackTrace();
        }
    }

    public void controlCPPMediaControl(String actionRcvd) {
        if (actionRcvd != null) {
            if (actionRcvd.equals("com.denkplay.states.action.prev")) {
                sendToQt("((((Prev))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.play")) {
                sendToQt("((((Play))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.pause")) {
            sendToQt("((((Pause))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.next")) {
                sendToQt("((((Next))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.suspend")) {
                sendToQt("((((Suspend))))");
            } else if (actionRcvd.equals("com.denkplay.states.action.unsuspend")) {
                sendToQt("((((Unsuspend))))");
            }
        }
    }


    // Binder given to clients.
    private final IBinder binder = new MyBinder();
    /**
     * Binder.getService() returns a service object.
     * To make Binder.getService() return the instance of this service,
     * we inherit from it, then we implement our custom MyBinder.getService()
     */
    public class MyBinder extends Binder {
        DenkService getService() {
            // Return this instance of DenkService so clients can call its public methods.
            return DenkService.this;
        }
    }
    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }


    // private boolean foregroundServiceIsRunning() {
    //     ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
    //     for (ActivityManager.RunningServiceInfo rService: activityManager.getRunningServices(Integer.MAX_VALUE)) {
    //         if (DenkService.class.getName().equals(rService.service.getClassName())) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

}
