/*
     File: MapViewController.h
*/

#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKAnnotation.h> 
#import <CoreLocation/CoreLocation.h>
#import "WorldCitiesListController.h"


@interface MapViewController : UIViewController <MKMapViewDelegate, WorldCitiesListControllerDelegate, UIAlertViewDelegate>
{
    MKMapView *mapView;
	UIAlertView *alertButton;	
    WorldCitiesListController *worldCitiesListController;
    UINavigationController *worldCitiesListNavigationController;
	NSMutableArray *mapAnnotations;
	NSMutableArray *pointAnnotations;
}

@property (nonatomic, retain) IBOutlet MKMapView *mapView;
@property (nonatomic, retain, readonly) WorldCitiesListController *worldCitiesListController;
@property (nonatomic, retain, readonly) UINavigationController *worldCitiesListNavigationController;
@property (nonatomic,retain) NSMutableArray *mapAnnotations;
@property (nonatomic,retain) NSMutableArray *pointAnnotations;

- (IBAction)setMapType:(id)sender;

@end