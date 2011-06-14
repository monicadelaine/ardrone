#import "RoiViewController.h"
#import "MapViewController.h"
#import "MyAnnotation.h"

enum
{
    kAnnotationIndex = 0
};

@implementation RoiViewController

@synthesize mapView, testCoord, waypoint, socket, socket2;

int enlargeFlag1 = 0;
int width=64, height=64;
NSString * host = @"130.160.221.212";
UInt16 iport = 1501;
UInt16 iport2 = 1503;

- (void) displayImage1:(unsigned char *)data
{
	//NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
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
	if (enlargeFlag1 == 0) {
		myImageRect1 = CGRectMake(0.0f, 328.0f, 162.0f, 88.0f); 
	} else if (enlargeFlag1 == 1) {
		myImageRect1 = CGRectMake(0.0f, 0.0f, 320.0f, 418.0f); 
	}
	UIImageView *myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
	//myImage1.backgroundColor = [UIColor blackColor];
	[myImage1 setImage:newImage1]; 
	myImage1.opaque = YES;  
	//int borderWidth = 10;
	//myImage1.frame = CGRectMake(borderWidth, borderWidth, 162.0, 88.0);
	[self.view addSubview:myImage1]; 
	[myImage1 release];	
	
	//[pool release];
}

- (void) displayImage2:(unsigned char *)data
{
	//NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
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
	CGRect myImageRect2 = CGRectMake(162.0f, 328.0f, 163.0f, 88.0f); 
	UIImageView *myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
	[myImage2 setImage:newImage2]; 
	myImage2.opaque = YES;  
	[self.view addSubview:myImage2]; 
	[myImage2 release];	
	
	//[pool release];
}

//connect to server
-(void)connectToSever
{
	NSLog(@"in connectToServer");
	NSError *error = nil;
	if ([socket connectToHost:host onPort:iport error:&error]) { // connect
		NSLog(@"Connected on port %i", iport);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport, error);
	}
	
	if ([socket2 connectToHost:host onPort:iport2 error:&error]) { // connect
		NSLog(@"Connected on port %i", iport2);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport2, error);
	}
	
	[self sendDataToServer:@"UAV Testing"]; //send data
	[self receiveDataFromServer]; //receive data
}

//send data to server
-(void)sendDataToServer:(NSString *)data
{
	NSLog(@"in sendDataToServer");
	[socket sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	[socket2 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	NSLog (@"Sending: %@",data);
}

//receive data from server
-(void)receiveDataFromServer
{
	NSLog(@"in receiveDataFromServer");
	[socket receiveWithTimeout:-1 tag:0];
	[socket2 receiveWithTimeout:-1 tag:0];
	NSLog (@"Receiving");
}

//Called when the datagram with the given tag has been sent
- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag 
{
	NSLog(@"in didSendDataWithTag");
}

//Called when state is received
- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    NSLog(@"in didReceiveData: %i", port);
	unsigned char *byteData = (unsigned char *)[data bytes];
	
	if (port == iport) {
		[self displayImage1:byteData]; 
	}
	if (port == iport2) {
		[self displayImage2:byteData]; 
	}
	[sock receiveWithTimeout:-1 tag:0]; 
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
			enlargeFlag1 = 0;
			break;
        } 
		case 1:
        {
			//action for waypoint 
			NSLog(@"Waypoint button");
			UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
			[self.mapView addGestureRecognizer:longPressGesture];
			[longPressGesture release];  	
			
			//send waypoint to server
			/*NSString *waypointStr = [waypoint description];
			[socket sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];*/
			break;
        } 
        default:
        {	
			//action for Enlarge
			NSLog(@"Enlarge button");	
			enlargeFlag1 = 1;
            break;
        } 
    }			
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	if (sender.state==UIGestureRecognizerStateBegan) {
		// get the CGPoint for the touch and convert it to latitude and longitude coordinates
		CGPoint point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
		//save the coordinates for the selected waypoint
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.latitude] atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.longitude]atIndex:kAnnotationIndex];	
		
		NSLog(@"count=%i", [waypoint count]);
		int count=[waypoint count];
		for (int i=0; i<count; i++) {
			NSLog(@"element %i = %@", i, [waypoint objectAtIndex:i]);
		}
	}				
} 

- (void)moveUAVs
{
	//[mapView removeAnnotations:mapView.annotations];  // remove any annotations that exist
	
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
	MKAnnotationView *annView = [theMapView dequeueReusableAnnotationViewWithIdentifier:identifer];
	if (annView==nil) {
		annView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:identifer] autorelease];
	}
	//annView.image = [UIImage imageNamed:@"Icon-Small.png"];
	UIImage *uavImage = [UIImage imageNamed:@"Icon-Small.png"];
	[annView setImage:uavImage];
	return annView;
}

- (void) setCoord:(NSMutableArray *)passedArray
{
	testCoord = passedArray;
	
	int cnt=[testCoord count];
	NSLog(@"count: %i",cnt);
		
	for (int i=0; i<cnt; i++) {
		NSLog(@"element %i = %@", i, [testCoord objectAtIndex:i]);
	}
}

- (void) viewDidLoad {
	self.waypoint = [[NSMutableArray alloc] init];
	
	//connect to server
	socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
	socket2 = [[AsyncUdpSocket alloc] initWithDelegate:self];	
	[self connectToSever];
	
	//[self startThread]; //start threads
	
	mapView.mapType = MKMapTypeSatellite;
	
	//get coordinates for the ROI 
	//NSLog(@"element = %@", [testCoord objectAtIndex:1]);
	
	//try visibleMapRect or
	//use convertRect:toRegionFromView to convert a rectangle to a map region
	//create function to get center of chosen ROI
	
	CLLocationCoordinate2D coord;
	coord.latitude=33.2137;
	coord.longitude=-87.5425;
	MKCoordinateRegion region;
	region.center=coord;
    MKCoordinateSpan span = {0.0005, 0.0005};
    region.span = span;
    [mapView setRegion:region animated:YES];
	
	[self moveUAVs];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload
{
	self.mapView = nil;
	self.testCoord = nil;
	self.waypoint = nil;
}

- (void)dealloc {
	[mapView release];
	[testCoord release];
	[waypoint release];
	[socket release];
	[socket2 release];
	
	[super dealloc];
}

/*- (void)startThread 
{
	[NSThread detachNewThreadSelector:@selector(displayImage1:) toTarget:self withObject:nil];
}*/

@end
