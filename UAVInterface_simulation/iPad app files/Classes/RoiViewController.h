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
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic, retain) NSMutableArray *initCoord;
@property (nonatomic, retain) NSMutableArray *initPoint;
@property (nonatomic, retain) NSMutableArray *pointWaypoint;
@property (nonatomic, retain) NSMutableArray *waypoint;
@property (nonatomic, retain) AsyncUdpSocket *socket;
@property (nonatomic, retain) AsyncUdpSocket *socket2;

- (IBAction)options:(id)sender;

- (void)connectToServer;
- (void)sendDataToServer:(NSString *)data;
- (void)receiveDataFromServer;
- (void)displayImage1:(unsigned char *)data;
- (void)displayImage2:(unsigned char *)data;
- (void)moveUAVs:(NSMutableArray *)data;
- (void)mapToArea:(double)x1:(double)y1;
- (void)mapToRoi:(double)x1:(double)y1:(double)x2:(double)y2;
- (void)setPoint:(NSMutableArray *)passedArray:(NSString *)passedStr;
- (void)setCoord:(NSMutableArray *)passedArray;

@end
