/*
	File: MapViewController.m
	Abstract: Controls the map view and sets the region according to the selected area. 
*/

#import "MapViewController.h"
#import "WorldCity.h"
#import "MyAnnotation.h"
#import "RoiViewController.h"

enum
{
    kAnnotationIndex = 0
};

@implementation MapViewController

@synthesize mapView, mapAnnotations, pointAnnotations;

NSString *pointStr;
float p1,p2,p3,p4,p5,p6,p7,p8;
int touchCount=0;

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
	self.pointAnnotations = [[NSMutableArray alloc] init];
}


- (void)viewDidUnload
{
	self.mapView = nil;
	self.mapAnnotations = nil;
	self.pointAnnotations = nil;	
	
	worldCitiesListController = nil;
	worldCitiesListNavigationController = nil;
}

- (void)dealloc
{
    [mapView release];
    [worldCitiesListController release];
    [worldCitiesListNavigationController release];
	[mapAnnotations release];
	[pointAnnotations release];	
	
    [super dealloc];
}

- (IBAction)reset:(id)sender
{
	//action for Reset
	NSLog(@"Reset button");		
	[mapView removeAnnotations:mapView.annotations];
	[mapAnnotations removeAllObjects];
	[pointAnnotations removeAllObjects];			
}

- (IBAction)areas:(id)sender
{
	//action for when user selects Areas
	NSLog(@"Areas button");
	[self.navigationController presentModalViewController:self.worldCitiesListNavigationController animated:YES];
}

- (IBAction)done:(id)sender
{
	//action for when user selects Done
	NSLog(@"Done button");
	if ([pointAnnotations count]<8) {
		alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Not enough points for ROI" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
		[alertButton show];
		[alertButton release];		
	} else {
		pointStr = [NSString stringWithFormat:@"0,%f,%f,%f,%f,%f,%f,%f,%f,254",p1,p2,p3,p4,p5,p6,p7,p8];
		RoiViewController *roi = [[RoiViewController alloc] init];
		roi.title = @"ROI";
		[roi setCoord:mapAnnotations];
		[roi setPoint:pointAnnotations:pointStr];
		[self.navigationController pushViewController:roi animated:YES];
		[roi release];		
	}
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if(alertView == alertButton) {
		NSLog(@"Not enough coordinates entered");
	}
}

- (void) regionCoord
{
	//saves boundary coordinates of region displayed
	NSMutableArray *testing = [[NSMutableArray alloc] init];
	MKCoordinateRegion r = mapView.region;
	//southwest coords
	[testing insertObject:[NSNumber numberWithDouble:r.center.latitude - (r.span.latitudeDelta/2)]atIndex:kAnnotationIndex];
	[testing insertObject:[NSNumber numberWithDouble:r.center.longitude - (r.span.longitudeDelta/2)]atIndex:kAnnotationIndex];
	//northeast coords
	[testing insertObject:[NSNumber numberWithDouble:r.center.latitude + (r.span.latitudeDelta/2)]atIndex:kAnnotationIndex];
	[testing insertObject:[NSNumber numberWithDouble:r.center.longitude + (r.span.longitudeDelta/2)]atIndex:kAnnotationIndex];
	int count=[testing count];
	for (int i=0; i<count; i++) {
		NSLog(@"element %i = %@", i, [testing objectAtIndex:i]);
	}
	[testing release];
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender {
	if (sender.state==UIGestureRecognizerStateBegan) {
		// get the CGPoint for the touch and convert it to latitude and longitude coordinates to display on the map
		CGPoint point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];

		//Testing Map points
		MKMapPoint p = MKMapPointForCoordinate(locCoord);
		NSLog(@"map point %f, %f ", p.x, p.y);	
		
		// create the annotation and add it to the map
		MyAnnotation *myAnnotation = [[MyAnnotation alloc] init];
		myAnnotation.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnnotation.longitude = [NSNumber numberWithDouble:locCoord.longitude];
		//save the coordinates for the selected ROI
		[self.mapAnnotations insertObject:myAnnotation.longitude atIndex:kAnnotationIndex];	
		[self.mapAnnotations insertObject:myAnnotation.latitude atIndex:kAnnotationIndex];
		[self.pointAnnotations insertObject:[NSNumber numberWithDouble:point.y] atIndex:kAnnotationIndex];	
		[self.pointAnnotations insertObject: [NSNumber numberWithDouble:point.x] atIndex:kAnnotationIndex];		
		[self.mapView addAnnotation:myAnnotation];
		[myAnnotation release];				
		
		int cnt=[mapAnnotations count];
		NSLog(@"coordinate %i = %@, %@ ", cnt/2, myAnnotation.latitude, myAnnotation.longitude);
		int pcnt=[pointAnnotations count];
		NSLog(@"point %i = %f, %f ", pcnt/2, point.x, point.y);		
		
		if (touchCount==0) {
			p1=point.x; p2=point.y;
		} else if (touchCount==1) {
			p3=point.x; p4=point.y;
		}else if (touchCount==2) {
			p5=point.x; p6=point.y;
		}else if (touchCount==3) {
			p7=point.x; p8=point.y;
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
