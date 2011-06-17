//
//  RoiViewController.h


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKAnnotation.h> 
#import <CoreLocation/CoreLocation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "AsyncUdpSocket.h"
#import "MapViewController.h"

@interface RoiViewController : UIViewController <MKMapViewDelegate, UIAlertViewDelegate>
{
    MKMapView *mapView;
	NSMutableArray *initCoord;
	NSMutableArray *initPoint;	
	NSMutableArray *waypoint;
	NSMutableArray *pointWaypoint;
	UIAlertView *alertButton;
	AsyncUdpSocket *socket;
	AsyncUdpSocket *socket2;
	AsyncUdpSocket *socket3;
	AsyncUdpSocket *socket4;
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic, retain) NSMutableArray *initCoord;
@property (nonatomic, retain) NSMutableArray *initPoint;
@property (nonatomic, retain) NSMutableArray *pointWaypoint;
@property (nonatomic, retain) NSMutableArray *waypoint;
@property (nonatomic, retain) AsyncUdpSocket *socket;
@property (nonatomic, retain) AsyncUdpSocket *socket2;
@property (nonatomic, retain) AsyncUdpSocket *socket3;
@property (nonatomic, retain) AsyncUdpSocket *socket4;

- (IBAction)options:(id)sender;

- (void)startThread;
- (void)connectToSever;
- (void)sendDataToServer:(NSString *)data;
- (void)receiveDataFromServer1;
- (void)receiveDataFromServer2;

@end

