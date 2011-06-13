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
	AsyncUdpSocket *socket1;
	//AsyncUdpSocket *socket2;
	//AsyncUdpSocket *socket3;
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic,retain) NSMutableArray *testCoord;
@property (nonatomic, retain) AsyncUdpSocket *socket;
//@property (nonatomic, retain) AsyncUdpSocket *socket2;
//@property (nonatomic, retain) AsyncUdpSocket *socket3;

- (IBAction)options:(id)sender;

- (void)startThread;
- (void)connectToSever;
- (void)sendDataToServer:(NSString *)data;
-(void)receiveDataFromServer;

@end
