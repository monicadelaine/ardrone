/*
     File: MapViewController.m
 Abstract: Controls the map view and sets the region according to the selected area.
  Version: 1.1
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2010 Apple Inc. All Rights Reserved.
 
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

- (IBAction)setMapType:(id)sender
{
    switch (((UISegmentedControl *)sender).selectedSegmentIndex)
    {
        case 0:
        {
            //action for Reset
			NSLog(@"Reset button");
			[mapView removeAnnotations:mapView.annotations];
			[mapAnnotations removeAllObjects];
            break;
        } 
        case 1:
        {
			//action for when user selects Areas
			NSLog(@"Areas button");
            [self.navigationController presentModalViewController:self.worldCitiesListNavigationController animated:YES];
            break;
        } 
        default:
        {
            //action for when user selects Done
			NSLog(@"Done button");
			RoiViewController *roi = [[RoiViewController alloc] initWithNibName:nil bundle:nil];
			[roi setCoord:mapAnnotations];
			[roi setPoint:pointAnnotations];
			[self presentModalViewController:roi animated:YES];
			[roi release];			
			break;
        } 
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
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender {
	if (sender.state==UIGestureRecognizerStateBegan) {
		// get the CGPoint for the touch and convert it to latitude and longitude coordinates to display on the map
		CGPoint point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
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
		NSLog(@"annotation count = %i",cnt);
		for (int i=0; i<cnt; i++) {
			NSLog(@"element %i = %@", i, [mapAnnotations objectAtIndex:i]);
		}
		int cnt1=[pointAnnotations count];
		NSLog(@"point annotation count = %i",cnt1);
		for (int i=0; i<cnt1; i++) {
			NSLog(@"element %i = %@", i, [pointAnnotations objectAtIndex:i]);
		}		
	}
}

- (void)worldCitiesListController:(WorldCitiesListController *)controller didChooseWorldCity:(WorldCity *)aPlace
{
    [self.navigationController dismissModalViewControllerAnimated:YES];
    self.title = aPlace.name;
	NSLog(@"%@ chosen",aPlace.name);
    MKCoordinateRegion current = mapView.region;
    if (current.span.latitudeDelta < 10)
    {
        [self performSelector:@selector(animateToWorld:) withObject:aPlace afterDelay:0.3];
        [self performSelector:@selector(animateToPlace:) withObject:aPlace afterDelay:1.7];        
    }
    else
    {
        [self performSelector:@selector(animateToPlace:) withObject:aPlace afterDelay:0.3];        
    }
}

-(MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{
	MKPinAnnotationView *annView =[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:nil];
	annView.animatesDrop=TRUE;
	return annView;
}
	
@end
