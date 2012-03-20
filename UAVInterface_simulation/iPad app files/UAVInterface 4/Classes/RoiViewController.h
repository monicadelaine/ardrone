//
//  RoiViewController.h


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKAnnotation.h> 
#import <CoreLocation/CoreLocation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "AsyncUdpSocket.h"


@interface RoiViewController : UIViewController <MKMapViewDelegate, UIAlertViewDelegate>
{
    MKMapView *mapView;	
	NSMutableArray *initCoord;	
	NSMutableArray *waypoint;
	NSMutableArray *pointWaypoint;	
	UIAlertView *alertButton;
	UIAlertView *alertButtonEnd;	
	AsyncUdpSocket *socket;
	AsyncUdpSocket *socket2;
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic, retain) NSMutableArray *initCoord;
@property (nonatomic, retain) NSMutableArray *pointWaypoint;
@property (nonatomic, retain) NSMutableArray *waypoint;
@property (nonatomic, retain) AsyncUdpSocket *socket;
@property (nonatomic, retain) AsyncUdpSocket *socket2;

- (IBAction)roi:(id)sender;
- (IBAction)done:(id)sender;

- (void)connectToServer;
- (void)sendDataToServer:(NSString *)data;
- (void)receiveDataFromServer;
- (void)displayImage1:(unsigned char *)data;
- (void)displayImage2:(unsigned char *)data;
- (void)displayImage3:(unsigned char *)data;
- (void)displayImage4:(unsigned char *)data;
- (void)moveUAVs:(int *)data;
- (void)targetInView:(int *)data;
- (void)wpReached:(int *)data;
- (float)distEq:(int)x1:(int)x2:(int)y1:(int)y2;
- (void)setPoint:(NSMutableArray *)passedCoord:(int)passedAlg:(NSString *)passedStr:(int)passedUAV;

@end
