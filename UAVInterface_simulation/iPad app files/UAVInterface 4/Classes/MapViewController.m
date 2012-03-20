/*
	File: MapViewController.m
	Abstract: Controls the map view and sets the region according to the selected area. 
*/

#import "MapViewController.h"
#import "WorldCity.h"
#import "MyAnnotation.h"
#import "RoiViewController.h"

#define MAPPOINTX 68940600
#define MAPPOINTY 107935300

enum {
    kAnnotationIndex = 0
};

@implementation MapViewController

@synthesize mapView, mapAnnotations, mapPointAnnotations;

NSString *pointStr;
float p1=0,p2=0,p3=0,p4=0,p5=0,p6=0,p7=0,p8=0;
int touchCount=0;
int algID = 1, numUAV = 2;

- (WorldCitiesListController *)worldCitiesListController
{
    if (worldCitiesListController == nil)
    {
        worldCitiesListController = [[WorldCitiesListController alloc] initWithStyle:UITableViewStylePlain];
        worldCitiesListController.delegate = self;
        worldCitiesListController.title = @"Choose an area to survey:";
    }
    return worldCitiesListController;
}

- (UINavigationController *)worldCitiesListNavigationController
{
    if (worldCitiesListNavigationController == nil)
    {
        worldCitiesListNavigationController = [[UINavigationController alloc] initWithRootViewController:self.worldCitiesListController];
    }
    return worldCitiesListNavigationController;
}

- (void)viewDidLoad
{
	mapView.mapType = MKMapTypeSatellite;
	self.mapAnnotations = [[NSMutableArray alloc] init];
	self.mapPointAnnotations = [[NSMutableArray alloc] init];
}

- (void)viewDidUnload
{
	self.mapView = nil;
	self.mapAnnotations = nil;	
	self.mapPointAnnotations = nil;
}

- (void)dealloc
{
    [mapView release];
    [worldCitiesListController release];
    [worldCitiesListNavigationController release];
	[mapAnnotations release];
	[mapPointAnnotations release];	
	
    [super dealloc];
}

- (IBAction)reset:(id)sender
{
	//action for Reset
	NSLog(@"Reset button");		
	[self resetAll];			
}

- (IBAction)areas:(id)sender
{
	//action for when user selects Areas
	NSLog(@"Areas button");
	[self resetAll];
	[self.navigationController presentModalViewController:self.worldCitiesListNavigationController animated:YES];
}

- (IBAction)done:(id)sender
{
	//action for when user selects Done
	NSLog(@"Done button");
	[self chooseAlgorithm];
}

- (void)chooseAlgorithm
{
	alertButton2 = [[UIAlertView alloc] initWithTitle:@"Which approach would you like to use?" message:@"" delegate:self cancelButtonTitle:@"Manual" otherButtonTitles:@"Autonomous", @"Semi-autonomous",nil];
	[alertButton2 show];
	[alertButton2 release];
}


- (void) pushView
{
	//pointStr = [NSString stringWithFormat:@"0,0,%f,%f,%f,%f,%f,%f,%f,%f,%i,254",p1,p2,p3,p4,p5,p6,p7,p8,algID];
	pointStr = [NSString stringWithFormat:@"0,0,%f,%f,%f,%f,%f,%f,%f,%f,%i,%i,254",p1,p2,p3,p4,p5,p6,p7,p8,algID,numUAV];	
	RoiViewController *roi = [[RoiViewController alloc] init];
	roi.title = @"ROI";
	[roi setPoint:mapAnnotations:algID:pointStr:numUAV];
	[self.navigationController pushViewController:roi animated:YES];
	[roi release];	
}


// check for the number of robots
- (void) uavCheck
{
	alertButton3 = [[UIAlertView alloc] initWithTitle:@"How many UAVs would you like to use?" message:@"" delegate:self cancelButtonTitle:@"4" otherButtonTitles:@"2", @"3",nil];
	[alertButton3 show];
	[alertButton3 release];
}


// check for the correct number of ROI points 
- (void) doneCheck
{
	if ([mapPointAnnotations count]==0) {
		[self pushView];
		/*alertButton = [[UIAlertView alloc] initWithTitle:@"Not enough points for ROI" message:@"Do you want to continue with default ROI?" delegate:self cancelButtonTitle:@"Yes" otherButtonTitles:@"No", nil];
		[alertButton show];
		[alertButton release];*/			
	} else if ([mapPointAnnotations count]!=8) {
		alertButton1 = [[UIAlertView alloc] initWithTitle:@"Incorrect number points for ROI" message:@"Please reset ROI and try again!" delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
		[alertButton1 show];
		[alertButton1 release];	
	} else {		
		[self pushView];		
	}
}


// options for alert buttons
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if(alertView == alertButton) {
		if(buttonIndex == 0) {
			NSLog(@"Use default ROI");
			[self pushView];
		} else {
			NSLog(@"Not enough coordinates entered");
		}
	} else if(alertView == alertButton1) {
		if(buttonIndex == 0) {
			NSLog(@"Incorrect number of points for ROI");
		} 
	} else if(alertView == alertButton2) {
		if(buttonIndex == 0) {
			NSLog(@"Use User Waypoints (manual)");
			algID = 1;
			[self uavCheck];
		} else if(buttonIndex == 1) {
			NSLog(@"Use QoS Metric (autonomous)");
			algID = 2;
			[self uavCheck];
		} else if(buttonIndex == 2) {
			NSLog(@"Use QoS + Waypoints (semi-autonomous)");
			algID = 3;
			[self uavCheck];
		}	
	} else if (alertView == alertButton3) {
		if(buttonIndex == 0) {
			NSLog(@"Use 4 UAVs");
			numUAV = 4;
			[self doneCheck];
		} else if(buttonIndex == 1) {
			NSLog(@"Use 2 UAVs");
			numUAV = 2;
			[self doneCheck];
		} else if(buttonIndex == 2) {
			NSLog(@"Use 3 UAVs");
			numUAV = 3;
			[self doneCheck];
		}		
	}
}


- (void)resetAll
{
	[mapView removeAnnotations:mapView.annotations];
	[mapAnnotations removeAllObjects];	
	[mapPointAnnotations removeAllObjects];	
	p1=0,p2=0,p3=0,p4=0,p5=0,p6=0,p7=0,p8=0;
	touchCount=0;
}


- (void)handleLongPressGesture:(UIGestureRecognizer*)sender {
	if (sender.state==UIGestureRecognizerStateBegan) {
		// get the CGPoint for the touch and convert it to latitude and longitude coordinates to display on the map
		CGPoint point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
		MKMapPoint p = MKMapPointForCoordinate(locCoord);
		// create the annotation and add it to the map
		MyAnnotation *myAnnotation = [[MyAnnotation alloc] init];
		myAnnotation.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnnotation.longitude = [NSNumber numberWithDouble:locCoord.longitude];
		//save the coordinates for the selected ROI
		[self.mapAnnotations insertObject:myAnnotation.longitude atIndex:kAnnotationIndex];	
		[self.mapAnnotations insertObject:myAnnotation.latitude atIndex:kAnnotationIndex];
		[self.mapPointAnnotations insertObject:[NSNumber numberWithDouble:p.y] atIndex:kAnnotationIndex];	
		[self.mapPointAnnotations insertObject: [NSNumber numberWithDouble:p.x] atIndex:kAnnotationIndex];			
		[self.mapView addAnnotation:myAnnotation];
		[myAnnotation release];				
		
		//int cnt=[mapAnnotations count];
		//NSLog(@"coordinate %i = %@, %@ ", cnt/2, myAnnotation.latitude, myAnnotation.longitude);	
		int mpcnt=[mapPointAnnotations count];
		NSLog(@"point %i = %f, %f ", mpcnt/2, p.x-MAPPOINTX, p.y-MAPPOINTY);			
		
		if (touchCount==0) {
			p1=p.x-MAPPOINTX; p2=p.y-MAPPOINTY;
		} else if (touchCount==1) {
			p3=p.x-MAPPOINTX; p4=p.y-MAPPOINTY;
		} else if (touchCount==2) {
			p5=p.x-MAPPOINTX; p6=p.y-MAPPOINTY;
		} else if (touchCount==3) {
			p7=p.x-MAPPOINTX; p8=p.y-MAPPOINTY;
		}
		touchCount++;
	}
}

- (void)animateToWorld:(WorldCity *)worldCity
{    
	MKCoordinateRegion current = mapView.region;
    MKCoordinateRegion zoomOut = {{(current.center.latitude + worldCity.coordinate.latitude)/2.0 , (current.center.longitude + worldCity.coordinate.longitude)/2.0 }, {90, 90}};
    [mapView setRegion:zoomOut animated:YES];
}

- (void)animateToPlace:(WorldCity *)worldCity
{
    MKCoordinateRegion region;
    region.center = worldCity.coordinate;
    MKCoordinateSpan span = {0.001, 0.001};
    region.span = span;
    [mapView setRegion:region animated:YES];
	
	UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
	[self.mapView addGestureRecognizer:longPressGesture];
	[longPressGesture release];
}

- (void)worldCitiesListController:(WorldCitiesListController *)controller didChooseWorldCity:(WorldCity *)aPlace
{
    [self.navigationController dismissModalViewControllerAnimated:YES];
    self.title = aPlace.name;
	NSLog(@"%@ chosen",aPlace.name);
    MKCoordinateRegion current = mapView.region;
    if (current.span.latitudeDelta < 10) {
        [self performSelector:@selector(animateToWorld:) withObject:aPlace afterDelay:0.3];
        [self performSelector:@selector(animateToPlace:) withObject:aPlace afterDelay:1.7];        
    }
    else {
        [self performSelector:@selector(animateToPlace:) withObject:aPlace afterDelay:0.3];        
    }
}

	
@end
