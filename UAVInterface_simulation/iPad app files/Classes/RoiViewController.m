//
//  RoiViewController.m

#import <QuartzCore/QuartzCore.h>

#import "RoiViewController.h"
#import "MapViewController.h"
#import "MyAnnotation.h"

enum
{
    kAnnotationIndex = 0,
	X_MAX = 318,
	Y_MAX = 371,
	ROI_X_MAX = 318,
	ROI_Y_MAX = 285
};

@implementation RoiViewController

@synthesize mapView, initCoord, initPoint, waypoint, pointWaypoint, socket, socket2, socket3, socket4;

double latSpan, lonSpan, centerCoordLat, centerCoordLon;

int enlargeFlag = 0, enlargeButton = 0, waypointUAV = 0, waypointButton = 0;
int width = 64, height = 64;
NSString * host = @"130.160.47.64";
UInt16 iport = 1501;
UInt16 iport2 = 1503;
UInt16 iport3 = 1505;
UInt16 oport4 = 1506;

- (void) displayImage1:(unsigned char *)data
{
	unsigned char *rawData1 = malloc(width*height*4);
	
	//adding alpha value to rgb data
	for (int i=0; i<width*height; ++i) {
		rawData1[4*i] = data[3*i];
		rawData1[4*i+1] = data[3*i+1];
		rawData1[4*i+2] = data[3*i+2];
		rawData1[4*i+3] = 255; //alpha
	}
	
	//convert rawData into a UIImage
	CGDataProviderRef provider1 = CGDataProviderCreateWithData(NULL,rawData1,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef1 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider1,NULL,NO,renderingIntent);
	UIImage *newImage1 = [UIImage imageWithCGImage:imageRef1];	
	
	//display image1
	CGRect myImageRect1;
	if (enlargeFlag == 0) {
		myImageRect1 = CGRectMake(0.0f, 328.0f, 162.0f, 88.0f); 
	} else if (enlargeFlag == 1) {
		myImageRect1 = CGRectMake(0.0f, 0.0f, 320.0f, 418.0f); 
	}
	UIImageView *myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
	[myImage1 setImage:newImage1]; 
	myImage1.opaque = YES;  
	if (enlargeFlag == 0) {
		myImage1.frame = CGRectMake(0.0, 328.0, 162.0, 88.0);
		myImage1.layer.borderColor = [UIColor redColor].CGColor;
		myImage1.layer.borderWidth = 1.0;
	} 	
	[self.view addSubview:myImage1]; 
	[myImage1 release];	
}

- (void) displayImage2:(unsigned char *)data
{
	unsigned char *rawData2 = malloc(width*height*4);
	
	//adding alpha value to rgb data
	for (int i=0; i<width*height; ++i) {
		rawData2[4*i] = data[3*i];
		rawData2[4*i+1] = data[3*i+1];
		rawData2[4*i+2] = data[3*i+2];
		rawData2[4*i+3] = 255; //alpha
	}
	
	//convert rawData into a UIImage
	CGDataProviderRef provider2 = CGDataProviderCreateWithData(NULL,rawData2,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef2 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider2,NULL,NO,renderingIntent);	
	UIImage *newImage2 = [UIImage imageWithCGImage:imageRef2];	
	
	//display image2
	CGRect myImageRect2;
	if (enlargeFlag == 0) {
		myImageRect2 = CGRectMake(162.0f, 328.0f, 162.0f, 88.0f); 
	} else if (enlargeFlag == 2) {
		myImageRect2 = CGRectMake(0.0f, 0.0f, 320.0f, 418.0f); 
	}	
	UIImageView *myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
	[myImage2 setImage:newImage2]; 
	myImage2.opaque = YES;  
	if (enlargeFlag == 0) {
		myImage2.frame = CGRectMake(162.0, 328.0, 162.0, 88.0);
		myImage2.layer.borderColor = [UIColor blueColor].CGColor;
		myImage2.layer.borderWidth = 1.0;
	} 	
	[self.view addSubview:myImage2]; 
	[myImage2 release];		
}

//connect to server
-(void)connectToSever
{
	NSLog(@"in connectToServer");
	NSError *error = nil;
	
	if ([socket connectToHost:host onPort:iport error:&error]) { // connect to socket1
		NSLog(@"Connected on port %i", iport);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport, error);
	}
	
	if ([socket2 connectToHost:host onPort:iport2 error:&error]) { // connect to socket2
		NSLog(@"Connected on port %i", iport2);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport2, error);
	}
	
	/*if ([socket connectToHost:host onPort:iport3 error:&error]) { // connect to socket3
		NSLog(@"Connected on port %i", iport3);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport3, error);
	}
	
	if ([socket2 connectToHost:host onPort:oport4 error:&error]) { // connect to socket4
		NSLog(@"Connected on port %i", oport4);
	} else {
		NSLog(@"Error connecting to server at %i: %@", oport4, error);
	}
	*/
	
	[self sendDataToServer:@"Initializing socket"]; //send data
	[self receiveDataFromServer1]; //receive data
	//[self receiveDataFromServer2];
}

//send data to server
-(void)sendDataToServer:(NSString *)data
{
	NSLog(@"in sendDataToServer");
	[socket sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	//[socket2 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	//[socket3 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	NSLog (@"Sending: %@",data);
}

//receive data from server
-(void)receiveDataFromServer1
{
	NSLog(@"in receiveDataFromServer1");
	
	//while (TRUE) {
		//NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		[socket receiveWithTimeout:-1 tag:0];
		//[socket2 receiveWithTimeout:-1 tag:0];
		//[self performSelectorOnMainThread:@selector withObject: waitUntilDone:
		//[pool release];
	//}
}

-(void)receiveDataFromServer2
{
	NSLog(@"in receiveDataFromServer2");
	
	//while (TRUE) {
		//NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		[socket2 receiveWithTimeout:-1 tag:0];
		//[pool release];
	//}
}

//Called when the datagram with the given tag has been sent
- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag 
{
	NSLog(@"in didSendDataWithTag");
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
	NSLog(@"in didNotSendDataWithTag %@", error);
}

//Called when state is received
- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    NSLog(@"in didReceiveData: %i", port);
	unsigned char *byteData = (unsigned char *)[data bytes];
	
	//if (port == iport) {
		//unsigned char *byteData = (unsigned char *)[data bytes];
		[self displayImage1:byteData]; 
	//}
	//if (port == iport2) {
		//unsigned char *byteData = (unsigned char *)[data bytes];
		//[self displayImage2:byteData]; 
	//}
	/*if (port == iport3) {
		NSArray *byteData = [NSKeyedArchiver unarchiveObjectWithData:data];
		[self moveUAVs:byteData]; 
	} */	
	[socket receiveWithTimeout:-1 tag:0]; 
	//[socket2 receiveWithTimeout:-1 tag:0]; 
	return YES;
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotReceiveDataWithTag:(long)tag dueToError:(NSError *)error {
	NSLog(@"in didNotReceiveDataWithTag %@", error);
}

- (IBAction)options:(id)sender 
{
	switch (((UISegmentedControl *)sender).selectedSegmentIndex)
    {
        case 0:
        {
			//action for ROI
			NSLog(@"ROI button");
			enlargeFlag = 0;
			break;
        } 
		case 1:
        {
			//action for waypoint 
			NSLog(@"Waypoint button");
			waypointButton=1;
			alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Waypoint for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
			[alertButton show];
			[alertButton release];
			
			UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
			[self.mapView addGestureRecognizer:longPressGesture];
			[longPressGesture release];  	
			break;
        } 
        case 2:
        {	
			//action for Enlarge
			NSLog(@"Enlarge button");
			enlargeButton=1;
			alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Enlarge video for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
			[alertButton show];
			[alertButton release];
            break;
        } 
    }			
}

- (void)alertView : (UIAlertView *)alertView clickedButtonAtIndex : (NSInteger)buttonIndex
{
	if(alertView == alertButton) {
		if(buttonIndex == 0) {
			NSLog(@"UAV1 button was pressed\n");
			if (enlargeButton==1) {
				enlargeFlag = 1;
			} else if (waypointButton==1) {
				waypointUAV = 1;
			}
			
		} else {
			NSLog(@"UAV2 button was pressed\n");
			if (enlargeButton==1) {
				enlargeFlag = 2;
			} else if (waypointButton==1) {
				waypointUAV = 2;
			}
		}
	}
	enlargeButton=0;
	waypointButton=0;
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	[waypoint removeAllObjects];
	if (sender.state==UIGestureRecognizerStateBegan) {
		// get the CGPoint for the touch and convert it to latitude and longitude coordinates
		CGPoint point = [sender locationInView:self.mapView];
		NSLog(@"point %f %f", point.x, point.y );
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
		//save the coordinates for the selected waypoint
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.longitude]atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.latitude] atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];
		[self.pointWaypoint insertObject:[NSNumber numberWithDouble:point.y] atIndex:kAnnotationIndex];	
		[self.pointWaypoint insertObject: [NSNumber numberWithDouble:point.x] atIndex:kAnnotationIndex];	
		
		NSLog(@"waypoint count=%i", [waypoint count]);
		int count=[waypoint count];
		for (int i=0; i<count; i++) {
			NSLog(@"element %i = %@", i, [waypoint objectAtIndex:i]);
		}
		int cnt1=[pointWaypoint count];
		NSLog(@"point waypoint count = %i",cnt1);
		for (int i=0; i<cnt1; i++) {
			NSLog(@"element %i = %@", i, [pointWaypoint objectAtIndex:i]);
		}			
	}	
	
	//send waypoint to server
	/*NSString *waypointStr = [waypoint description];
	[socket4 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];	*/	
	/*NSData *waypointData = [NSKeyedArchiver archivedDataWithRootObject:waypoint];
	 [socket4 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];	*/	
} 

- (void)moveUAVs:(NSArray *)data
{
	[mapView removeAnnotations:mapView.annotations]; 
	
	MyAnnotation *myAnn1 = [[MyAnnotation alloc] init];
	myAnn1.latitude=[NSNumber numberWithDouble:33.21371];
	myAnn1.longitude=[NSNumber numberWithDouble:-87.5422];		
	[mapView addAnnotation:myAnn1];	
	
	MyAnnotation *myAnn2 = [[MyAnnotation alloc] init];
	myAnn2.latitude=[NSNumber numberWithDouble:33.2137];
	myAnn2.longitude=[NSNumber numberWithDouble:-87.5425];	
	[mapView addAnnotation:myAnn2];
}

- (MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{ 
	NSString *identifer = @"UAV";
	MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:identifer] autorelease];
	UIImage *uavImage = [UIImage imageNamed:@"uav.png"];
	uavView.image=uavImage;
	uavView.opaque = YES;  
	[uavImage release];	
	return uavView;
}

- (void) setPoint:(NSMutableArray *)passedArray
{
	initPoint = passedArray;
	
	int cnt=[initPoint count];
	if (cnt!=0) {
		for (int i=0; i<cnt; i++) {
			NSLog(@"element %i = %@", i, [initPoint objectAtIndex:i]);
		}
	}
	
	//send initial coordinates of ROI to server
	/*NSString *coordStr = [testCoord description];
	 [socket4 sendData:[coordStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];*/
	
	/*if (cnt==0) { //default coordinates
		NSMutableArray *testing = [[NSMutableArray alloc] init];
		[testing insertObject:[NSNumber numberWithDouble:-87.54307508468628]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21366572356165] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54226505756378]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21367245565067] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54225432872772]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21305983432983] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54308313131332]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21306656645545] atIndex:kAnnotationIndex];
		NSString *coordStr = [testing description];
		[socket4 sendData:[coordStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	}*/
}

- (void) setCoord:(NSMutableArray *)passedArray
{
	initCoord = passedArray;
	
	int cnt=[initCoord count];
	if (cnt!=0) {
		for (int i=0; i<cnt; i++) {
			NSLog(@"element %i = %@", i, [initCoord objectAtIndex:i]);
		}
	}
	
	//send initial coordinates of ROI to server
	/*NSString *coordStr = [testCoord description];
	[socket4 sendData:[coordStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];*/
	
	if (cnt==0) { //default coordinates
		NSMutableArray *testing = [[NSMutableArray alloc] init];
		[testing insertObject:[NSNumber numberWithDouble:-87.54307508468628]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21366572356165] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54226505756378]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21367245565067] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54225432872772]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21305983432983] atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:-87.54308313131332]atIndex:kAnnotationIndex];
		[testing insertObject:[NSNumber numberWithDouble:33.21306656645545] atIndex:kAnnotationIndex];
		NSString *coordStr = [testing description];
		[socket4 sendData:[coordStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	}
}

- (double) calcDist:(int)x1:(int)y1:(int)x2:(int)y2
{
	double x1_ = [[initCoord objectAtIndex:x1] doubleValue];
	double y1_ = [[initCoord objectAtIndex:y1] doubleValue];
	double x2_ = [[initCoord objectAtIndex:x2] doubleValue];	
	double y2_ = [[initCoord objectAtIndex:y2] doubleValue];
	
	double x = pow(x2_ - x1_, 2.0);
	double y = pow(y2_ - y1_, 2.0);
	double dist = sqrt((x+y));
	
	return dist;
}

- (double) centerPt:(int)x1:(int)x2
{
	double x1_ = [[initCoord objectAtIndex:x1] doubleValue];
	double x2_ = [[initCoord objectAtIndex:x2] doubleValue];
	
	double x = (x2_ + x1_) / 2.0;
	
	return x;
}

- (void) regionSet
{
	int cnt=[initCoord count];
	if (cnt!=0) {
		latSpan = [self calcDist:0:1:6:7];
		lonSpan = [self calcDist:0:1:2:3];
		centerCoordLat = [self centerPt:6:2];
		centerCoordLon = [self centerPt:7:3];
	}
}

- (void) setMap
{
	//uses coordinates chosen by user to display ROI
	NSLog(@"ROI params: %f %f %f %f", latSpan, lonSpan, centerCoordLat, centerCoordLon);
	
	mapView.mapType = MKMapTypeSatellite;

	CLLocationCoordinate2D coord;
	MKCoordinateRegion region;
	if ([initCoord count]==0) {
		coord.latitude=33.2137;
		coord.longitude=-87.5425;	
		region.center=coord;
		MKCoordinateSpan span = {0.0005, 0.0005};
		region.span = span;
	} else {
		coord.latitude=centerCoordLat;
		coord.longitude=centerCoordLon;
		region.center=coord;
		MKCoordinateSpan span = {latSpan, lonSpan};
		region.span = span;
	}
	[mapView setRegion:region animated:YES];
}

- (void) viewDidLoad {
	[self regionSet]; //set ROI region
	
	self.waypoint = [[NSMutableArray alloc] init];
	self.pointWaypoint = [[NSMutableArray alloc] init];
	
	//connect to server
	socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
	socket2 = [[AsyncUdpSocket alloc] initWithDelegate:self];	
	/*socket3 = [[AsyncUdpSocket alloc] initWithDelegate:self];
	socket4 = [[AsyncUdpSocket alloc] initWithDelegate:self];*/
		
	[self connectToSever];
	
	//[self startThread]; //start threads
	
	mapView.delegate=self;
	
	[self setMap];
	
	[self mapping:105.0:135.0];	
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload
{
	self.mapView = nil;
	self.initCoord = nil;
	self.waypoint = nil;
}

- (void)dealloc {
	[mapView release];
	[initCoord release];
	[waypoint release];
	[socket release];
	[socket2 release];
	[socket3 release];
	[socket4 release];	
	
	[super dealloc];
}

/*- (void)startThread 
{
	while (TRUE) {
		[NSThread detachNewThreadSelector:@selector(receiveDataFromServer1) toTarget:self withObject:nil];
	}
	//[NSThread detachNewThreadSelector:@selector(receiveDataFromServer2) toTarget:self withObject:nil];
	[NSThread detachNewThreadSelector:@selector(receiveDataFromServer3) toTarget:self withObject:nil];
}*/

- (void) mapping:(double)x1:(double)y1
{
	double xScale = (double)ROI_X_MAX / ([[initPoint objectAtIndex:4] doubleValue] - [[initPoint objectAtIndex:6] doubleValue]);
	double yScale = (double)ROI_Y_MAX / ([[initPoint objectAtIndex:3] doubleValue] - [[initPoint objectAtIndex:5] doubleValue]);
	NSLog(@"scale: x=%f y=%f",xScale,yScale);
		
	double newX = xScale * (x1-[[initPoint objectAtIndex:6] doubleValue]);
	double newY = yScale * (y1-[[initPoint objectAtIndex:7] doubleValue]);
	NSLog(@"converted to %f %f to %f %f",x1,y1,newX,newY);
}
	
@end
