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
	AsyncUdpSocket *socket;
	AsyncUdpSocket *socket2;
	UIImageView *imageView;
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic, retain) NSMutableArray *initCoord;
@property (nonatomic, retain) NSMutableArray *pointWaypoint;
@property (nonatomic, retain) NSMutableArray *waypoint;
@property (nonatomic, retain) AsyncUdpSocket *socket;
@property (nonatomic, retain) AsyncUdpSocket *socket2;
@property (nonatomic, retain) UIImageView *imageView;

- (IBAction)roi:(id)sender;
- (IBAction)waypoint:(id)sender;
- (IBAction)enlarge:(id)sender;
- (IBAction)done:(id)sender;

- (void)connectToServer;
- (void)sendDataToServer:(NSString *)data;
- (void)receiveDataFromServer;
- (void)displayImage1:(unsigned char *)data;
- (void)displayImage2:(unsigned char *)data;
- (void)moveUAVs:(NSMutableArray *)data;
- (void)targetInView:(int *)data;
- (void)wpReached:(int *)data;
- (void)setPoint:(NSMutableArray *)passedCoord:(NSMutableArray *)passedPoint:(NSString *)passedStr;

@end
