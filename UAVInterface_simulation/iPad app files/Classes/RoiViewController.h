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

@interface RoiViewController : UIViewController <MKMapViewDelegate, MapControllerDelegate>
{
    MKMapView *mapView;
	MapViewController *mapAnn;
	NSMutableArray *testCoord;
	AsyncUdpSocket *socket;
	UIImage *uavImage;
}

@property (nonatomic, retain) IBOutlet UIImage *uavImage;
@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic,retain) NSMutableArray *testCoord;
@property (nonatomic, retain) AsyncUdpSocket *socket;

- (IBAction)options:(id)sender;

- (void)openUDPToSever;
- (void)sendDataToServer:(NSString *)data;
- (void)receiveDataFromServer;

@end
