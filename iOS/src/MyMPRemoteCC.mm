#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
// #import <UIKit/UIKit.h>
// #import <MobileCoreServices/MobileCoreServices.h>
// #include <stdio.h>
#import "MyMPRemoteCC.h"

// Un-initialized pointer to cpp class.
// This should be a the very top because
// it will be called from within @implementation
// and also within c-style functions.
// It will be inited from initAllObjectiveCBridges
// at app launch.
Top *cppObject;

/**
  * MyMPRemoteCC class...
  * @param none
  * @returns none
  */
@interface MyMPRemoteCC : NSObject <UIDocumentPickerDelegate>
{
  NSNotificationCenter *notifCenter;        // for adding and removing observers AKA notification for audio state-change listeners
  MPRemoteCommandCenter *myCommandCenter;   // For holding the control center instance

@public /* This is for member vars that needs to be declared public. Here we have none. */
}
- (void)listenForInterruptions;
- (void)setupMPRemoteCommandCenter;
- (MPRemoteCommandHandlerStatus)previousAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)nextAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)playPauseAudio: (MPRemoteCommandHandlerStatus *)event;
- (void)passAudioDetailsToInfoCenter: (NSString *)title :(NSString *)artist;
- (void)openDocumentPicker;
- (void)documentPicker: (UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls;

@end
@implementation MyMPRemoteCC
// class constructor (id means instancetype AKA MyMPRemoteCC)
-(id)init
{
  // Initialize base class before this AKA self
  // IF PROBLEM, SIMPLY COMMENT OUT COZ APP WORKED FINE WITHOUT IT.
  // self = [super init];
  // if (!self) {
  //   return nil;
  // }

  return self;
}

// class destructor
-(void)dealloc
{
  // unplug the listeners.
  // removeObserver:self will remove all observers
  // observing self even if they are one million
  [notifCenter removeObserver:self];

  // removing any previous hold of this app on control center
  [myCommandCenter.togglePlayPauseCommand removeTarget:self];
  [myCommandCenter.togglePlayPauseCommand setEnabled:NO];

  [super dealloc];  // call the destructor of NSObject AKA parent that we inherited from
}


/**
  * This method is called when app launches on iOS.
  * It is called from initMyMPRemoteCC() at the time
  * when the app is instantiating the objective-c
  * part of the app.
  * It simply sets up a listener to listen for:
  * (1) interrupts AKA phone calls, alarms or any
  * other system sound that will cause this app to
  * relinquish audiosession,
  * (2) Duckings AKA when another audio player takes
  * control of audiosession.
  * NOTES:
  * (1) This listener will pick up notifications about
  * System interruptions even if the app is not playing
  * a song.
  * (2) But this listener can only pick up notifications
  * about Duckings only if the app was playing an audio at the
  * time the other audio player takes control of audiosession.
  * @param none
  * @returns void
  */
- (void)listenForInterruptions {
  [[AVAudioSession sharedInstance] setCategory:
                  AVAudioSessionCategoryPlayback
                  withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetooth | AVAudioSessionCategoryOptionAllowBluetoothA2DP | AVAudioSessionCategoryOptionAllowAirPlay
                  error:nil];

  @try {
    // Listen for system interrupts and duckings and suspend/unsuspend play accordingly
    notifCenter = [NSNotificationCenter defaultCenter];
    [notifCenter  addObserver:
                  self
                  selector:@selector(onAudioSessionInterrupted:) //wow! if the callback method takes an arg and you fail to add the appending ":" to its name, app will crash at runtime when it gets here
                  name:AVAudioSessionInterruptionNotification
                  object:[AVAudioSession sharedInstance]];
    //The below is for appending listener for bluetooth connection.
    // if you fail to add "withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetoothA2DP | AVAudioSessionCategoryOptionAllowAirPlay"
    // beforenow, it will not fire properly besides firng on app launch.
    [notifCenter  addObserver:
                  self
                  selector:@selector(onAudioSessionInterrupted:)
                  name:AVAudioSessionRouteChangeNotification
                  object:[AVAudioSession sharedInstance]];
  }
  @catch (NSException *exception) {
    // NSLog(@"%@", exception);
  }
}
/**
  * This method is callback passed to
  * listenForInterruptions() above.
  * It reads the notification to see whether
  * it is begin or end of notification, then
  * decides whether audio should be
  * suspended or unsuspended.
  * @param none
  * @returns void
  */
- (void) onAudioSessionInterrupted: (NSNotification *) notification
{
  // This means it is either a system interrupt or ducking by other audio apps
  if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification]) {
    NSDictionary *userInfo = notification.userInfo;

    //Check to see if it was a Begin interruption
    if ([[userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
      // Suspend your audio
      cppObject->suspendAudio();
    } else if([[userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeEnded]]){
      // This block is intended to resume play only if it is ok to do so.
      // AKA system interrupts can resume, but docking by z.B VLC shouldnt
      // because if it resumes after vlc ducked it, it will lose audio and the
      // only work around in that case is to wait for at least 6 seconds after
      // ducking interrupt ends before resuming. Even at that, it still wont
      // resume from background. That is why iOS system forbids it.
      NSNumber *optionsValue = userInfo[AVAudioSessionInterruptionOptionKey];
      AVAudioSessionInterruptionOptions options = (AVAudioSessionInterruptionOptions)[optionsValue unsignedIntegerValue];
      if (options & AVAudioSessionInterruptionOptionShouldResume) {
        // Unsuspend your audio
        cppObject->unsuspendAudio();
      }
    }
  }
  // This means a headset device (bluetooth or usb audio to speaker) interruption
  else if ([notification.name isEqualToString:AVAudioSessionRouteChangeNotification]) {
    NSDictionary *userInfo = notification.userInfo;
    NSNumber *reasonValue = userInfo[AVAudioSessionRouteChangeReasonKey];

    if (reasonValue) {
      AVAudioSessionRouteChangeReason reason = (AVAudioSessionRouteChangeReason)[reasonValue unsignedIntegerValue];
      switch (reason) {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
          // plugging in a headset, has made a preferred audio route available
          // Unsuspend your audio
          cppObject->unsuspendAudio();
          break;
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
          // unplugging a headset, now users audio privacy must be respected
          // Suspend your audio
          cppObject->suspendAudio();
          break;
        default:
          break;
      }
    }
  }
}


//
- (void)setupMPRemoteCommandCenter {
  // MPRemoteCommandCenter.sharedCommandCenter returns a pointer to the running instance.
  // This by itself wont turn on the lockscreen media control board.
  // At least one command must be setEnabled:YES and having a valid action:@selector
  // What it does is it seizes control of the lockscreen media control board for your app.
  myCommandCenter = [MPRemoteCommandCenter sharedCommandCenter];

  // Add handler for Commands we intend to support
  [myCommandCenter.previousTrackCommand setEnabled:YES];
  [myCommandCenter.previousTrackCommand addTarget:self action:@selector(previousAudio:)];
  //
  [myCommandCenter.nextTrackCommand setEnabled:YES];
  [myCommandCenter.nextTrackCommand addTarget:self action:@selector(nextAudio:)];
  //
  // Play and pause buttons (bluetooth earphone control)
  [myCommandCenter.togglePlayPauseCommand setEnabled:YES];
  [myCommandCenter.togglePlayPauseCommand addTarget:self action:@selector(playPauseAudio:)];

  // explicitly disabling the skip forward and backward commands
  [myCommandCenter.skipBackwardCommand setEnabled:NO];
  [myCommandCenter.skipForwardCommand setEnabled:NO];

  // // Drag the progress bar https://programmersought.com/article/44296352753/
  // if (@available(iOS 9.1, *)) {
  //     MPRemoteCommand *changePlaybackPositionCommand = commandCenter.changePlaybackPositionCommand;
  //     changePlaybackPositionCommand.enabled = NO;
  //     [changePlaybackPositionCommand addTargetWithHandler:^MPRemoteCommandHandlerStatus(MPRemoteCommandEvent * _Nonnull event) {
  //         return MPRemoteCommandHandlerStatusCommandFailed;
  //     }];
  // } else {
  //     // Fallback on earlier versions
  // }
}

- (MPRemoteCommandHandlerStatus)previousAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->changePlay(false);
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)nextAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->changePlay(true);
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)playPauseAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->playOrPause();
  return MPRemoteCommandHandlerStatusSuccess;
}

// Not yet implemented
// THIS METHOD SHOULD BE CALLED ONLY AT THE VERY BEGINNING OF AN AUDIO PLAY. AKA WHEN NEXT IS CLICKED OR WHEN TRACK AUTO MOVED TO NEXT
// CALLING IT FROM EACH PLAY SIGNAL MEANS THE SEEKER WILL BEGIN FROM BEGINNING AFTER EACH PAUSE/PLAY
- (void)passAudioDetailsToInfoCenter: (NSString *)title
                                      :(NSString *)artist {
  // Retrieve the instance of MPNowPlayingInfoCenter
  MPNowPlayingInfoCenter *nowPlayingInfoCenter = [MPNowPlayingInfoCenter defaultCenter];

  // get the image from func parameter
  UIImage *artworkImage = [UIImage imageNamed:@"images/artist2.png"];
  // if no image received as func arg, use default from bundle
  artworkImage = (artworkImage) ? artworkImage : [UIImage imageNamed:@"images/artist.png"];
  // convert the UIImage into format that InfoCenter can display
  MPMediaItemArtwork *albumArt = [[MPMediaItemArtwork alloc] initWithBoundsSize:artworkImage.size requestHandler:^UIImage *(CGSize size) {
                  return artworkImage;
                }];
  // just pass an array/object directly to nowPlayingInfoCenter.nowPlayingInfo.
  // Assigning to nowPlayingInfoCenter.nowPlayingInfo will auto-update the Info Center
  nowPlayingInfoCenter.nowPlayingInfo = @{
      MPMediaItemPropertyTitle: title,
      MPMediaItemPropertyArtist: artist,
      MPMediaItemPropertyPlaybackDuration: @(120.0),
      MPMediaItemPropertyArtwork: albumArt // working
    };

  [albumArt release];
}


/**
  * Calling this method pops out the file browser so
  * the user can select.
  * @param
  * @returns void
  */
- (void)openDocumentPicker
{
  //This is needed, when using this code on QT!
  //Find the current app window, and its view controller object
  // retrieve the active UI instance
  /*UIApplication * app = [UIApplication sharedApplication]; //DEPRECATED*/
  UIWindowScene * app = (UIWindowScene *) ([UIApplication sharedApplication].connectedScenes.allObjects[0]);
  UIWindow * rootWindow = app.windows[0];
  UIViewController * rootViewController = rootWindow.rootViewController;

  //Initialize the document picker. Set appropriate document types
  //When reading: use document type of the file, that you're going to read
  //When writing into a new file: use @"public.folder" to select a folder, where your new file will be created
  // @[@"public.content",@"public.text",@"public.mp4"];
  UIDocumentPickerViewController *documentPicker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes:@[[UTType typeWithFilenameExtension:@"mp3"]]]; //crashes the program. Therefore, will stick to
  // UIDocumentPickerViewController *documentPicker = [[UIDocumentPickerViewController alloc] initWithDocumentTypes:@[@"public.mp3"] inMode:UIDocumentPickerModeOpen];

  // Assigning the delegate. We tell the document picker object to look
  // inside "this" when looking for callback method we will pass to it
  documentPicker.delegate = self;

  documentPicker.allowsMultipleSelection = YES;
  documentPicker.modalPresentationStyle = UIModalPresentationFormSheet;

  // If using iOS in fully separate controller classes, we use self.
  // [self presentViewController:documentPicker animated:YES completion:nil];
  // But we on QT, thus use the rootViewController we've found before instead
  [rootViewController presentViewController:documentPicker animated:YES completion:nil];
}
/**
  * This method is the callback used by above openDocumentPicker().
  * @param UIDocumentPickerViewController*, NSArray<NSURL *> *
  * @returns void
  */
- (void)documentPicker: (UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
  // If we come here, user successfully picked .mp3 file(s).
  // Now we loop through the array, and copy to our sandbox/tmp

  // But first we acquire the path to our app's sandbox/tmp
  NSString * mySandboxTmpDir = NSTemporaryDirectory();
  QString mySandboxTmpDir_cStr = [mySandboxTmpDir UTF8String];

  // Here we're creating vars in order to avoid creating
  // them multiple times inside a loop
  QString tempFilePath;


  for (NSURL * oneUrl in urls) {
    if ([oneUrl startAccessingSecurityScopedResource]) // Let iOS know we're going use this Url
    {
      // NSLog(@"startAccessingSecurityScopedResource SUCCESS");
      // NSLog(@"the one url:%@", oneUrl);

      // convert the oneUrl to nsstring to cstring and then to qstring
      QString oneUrlToQstring = [[oneUrl path] UTF8String];

      if(QFile::exists(oneUrlToQstring)) {
        qsizetype pos = oneUrlToQstring.lastIndexOf("/");
        tempFilePath = oneUrlToQstring.sliced(pos + 1);
        tempFilePath.prepend(mySandboxTmpDir_cStr);

        // copy to our app's sandbox/tmp dir only if similar file name
        // not yet existing there.
        if (! QFile::exists(tempFilePath)) {
          QFile::copy(oneUrlToQstring, tempFilePath);
        }
      }

      // Let iOS know we're done using this Url
      [oneUrl stopAccessingSecurityScopedResource];
    }
  }

  // Now call cpp part to refresh its audio list index
  // cppObject->doAddDir();
  cppObject->addDir(mySandboxTmpDir_cStr);
}

@end


/************* C functions for controlling this objective-c classes from cpp BEGINS *****/

// Un-initialized pointer to this objective-c class
MyMPRemoteCC *mpRmObject;


/**
  * This function is called when app launches on iOS.
  * It will simply initialize MyMPRemoteCC pointer.
  * MyMPRemoteCC is a pointer to this class and
  * through this pointer, we can call the methods
  * of this class from cpp side of codes using
  * c-style functions forward-declared inside
  * MyMPRemoteCC.h header.
  * @param instance of cpp part of this app
  * @returns void
  */
void initMyMPRemoteCC(Top *cppObj) {
  mpRmObject = [MyMPRemoteCC new];
  cppObject = cppObj;

  [mpRmObject listenForInterruptions];
}

void destroyMyMPRemoteCC() {
  [mpRmObject release];
}

void seizeControlOfInfoCenter() {
  [mpRmObject setupMPRemoteCommandCenter];
}

void updateInfoCenter(const char * title,
                      const char * artist) {
  [mpRmObject passAudioDetailsToInfoCenter:
                    [NSString stringWithCString:title encoding:NSUTF8StringEncoding]
                    :[NSString stringWithCString:artist encoding:NSUTF8StringEncoding]
                  ];
}

/**
  * This function is called from cpp when
  * user wants to select audio files for play.
  * @param
  * @returns void
  */
void openIosFileDialog() {
  [mpRmObject openDocumentPicker];
}
