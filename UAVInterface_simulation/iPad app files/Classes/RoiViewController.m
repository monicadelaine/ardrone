#import "RoiViewController.h"
#import "MapViewController.h"
#import "MyAnnotation.h"

enum
{
    kAnnotationIndex = 0
};

@implementation RoiViewController

@synthesize mapView, testCoord, socket;

int width=64, height=64;
NSString * host = @"130.160.250.5";
UInt16 port = 1501;


- (void) displayImage:(unsigned char *)data
{
	//NSLog(@"in displayImage");
	
	unsigned char *rawData1 = malloc(width*height*4);
	unsigned char *rawData2 = malloc(width*height*4);
	
	//adding alpha value to rgb data
	for (int i=0; i<width*height; ++i) {
		rawData1[4*i] = 255;
		rawData1[4*i+1] = 255;
		rawData1[4*i+2] = 255;
		rawData1[4*i+3] = 255; //alpha
	}
	for (int i=0; i<width*height; ++i) {
		rawData2[4*i] = data[3*i];
		rawData2[4*i+1] = data[3*i+1];
		rawData2[4*i+2] = data[3*i+2];
		rawData2[4*i+3] = 255; //alpha
	}
	
	//convert rawData into a UIImage
	CGDataProviderRef provider1 = CGDataProviderCreateWithData(NULL,rawData1,width*height*4,NULL);
	CGDataProviderRef provider2 = CGDataProviderCreateWithData(NULL,rawData2,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef1 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider1,NULL,NO,renderingIntent);
	UIImage *newImage1 = [UIImage imageWithCGImage:imageRef1];	
	
	//display image1
	CGRect myImageRect1 = CGRectMake(0.0f, 328.0f, 162.0f, 88.0f); 
	UIImageView *myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
	[myImage1 setImage:newImage1]; 
	myImage1.opaque = YES;  
	[self.view addSubview:myImage1]; 
	[myImage1 release];	
	
	CGImageRef imageRef2 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider2,NULL,NO,renderingIntent);	
	UIImage *newImage2 = [UIImage imageWithCGImage:imageRef2];	
	
	//display image2
	CGRect myImageRect2 = CGRectMake(162.0f, 328.0f, 162.0f, 88.0f); 
	UIImageView *myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
	[myImage2 setImage:newImage2]; 
	myImage2.opaque = YES;  
	[self.view addSubview:myImage2]; 
	[myImage2 release];	
}

//connect to server
-(void)connectToSever
{
	NSLog(@"in connectToServer");
	NSError *error = nil;
	if ([socket connectToHost:host onPort:port error:&error]) { // connect
		NSLog(@"Connected to Server");
		[self sendDataToServer:@"UAV Testing"]; //send data
	} else {
		NSLog(@"Error connecting to Server: %@", error);
	}
	[self receiveDataFromServer]; //receive data
}

//send data to server
-(void)sendDataToServer:(NSString *)data
{
	NSLog(@"in sendDataToServer");
	[socket sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	NSLog (@"Sending: %@",data);
}

//receive data from server
-(void)receiveDataFromServer
{
	NSLog(@"in receiveDataFromServer");
	[socket receiveWithTimeout:-1 tag:0];
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
    //NSLog(@"in didReceiveData");
	unsigned char *byteData = (unsigned char *)[data bytes];
	
	[self displayImage:byteData]; 
	[socket receiveWithTimeout:-1 tag:1]; 
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
            //action for Enlarge
			NSLog(@"Enlarge button");
            break;
        } 
		case 1:
        {
			//action for waypoint 
			NSLog(@"Waypoint button");
			UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
			[self.mapView addGestureRecognizer:longPressGesture];
			[longPressGesture release];  	
			break;
        } 
        default:
        {	
			//action for ROI
			NSLog(@"ROI button");
			break;
        } 
    }			
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender {
	// get the CGPoint for the touch and convert it to latitude and longitude coordinates 
	CGPoint point = [sender locationInView:self.mapView];
	CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
	CLLocationCoordinate2D touchCoord;
	touchCoord.latitude=locCoord.latitude;
	NSLog(@"lat=%i", touchCoord.latitude);
	//locCoord.latitude =[NSNumber numberWithDouble:locCoord.latitude];
	//locCoord.longitude = [NSNumber numberWithDouble:locCoord.longitude];
	//save the coordinates for the selected ROI
	//[self.testCoord insertObject:locCoord.latitude atIndex:kAnnotationIndex];
	//[self.testCoord insertObject:locCoord.longitude atIndex:kAnnotationIndex];				
	
	/*NSLog(@"count=%i", [testCoord count]);
	 int count=[testCoord count];
	 for (int i=0; i<count; i++) {
	 NSLog(@"element %i = %@", i, [testCoord objectAtIndex:i]);
	 }*/
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

-(MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{
	NSString *identifer = @"UAV";
	MKAnnotationView *annView = [theMapView dequeueReusableAnnotationViewWithIdentifier:identifer];
	if (annView==nil) {
		annView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:identifer] autorelease];
	}
	annView.image = [UIImage imageNamed:@"Icon-Small.png"];
	//UIImage *uavImage = [UIImage imageNamed:@"Icon-Small.png"];
	//[annView setImage:uavImage];
	return annView;
}

- (void)mapController:(MapViewController *)mapController didSendArray:(NSMutableArray *)array
{
	/*NSLog(@"count=%i", [array count]);
	 int count=[array count];
	 for (int i=0; i<count; i++) {
	 NSLog(@"element %i = %@", i, [array objectAtIndex:i]);
	 }*/
}

-(void) viewDidLoad {
	//[self didSendArray];
	unsigned char * test;
	[self displayImage:test];
	
	socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
	
	//connect to server
	[self connectToSever];
	
	mapView.mapType = MKMapTypeSatellite;
	mapAnn = [[MapViewController alloc] init];
	//try visibleMapRect
	//self.testCoord = [[NSMutableArray alloc] init];
	//hardcoded coordinates of ROI
	/*[testCoord addObject:[NSNumber numberWithFloat:33.21389012591766f]];
	 [testCoord addObject:[NSNumber numberWithFloat:-87.54267275333404f]];
	 [testCoord addObject:[NSNumber numberWithFloat:33.213887881896944f]];
	 [testCoord addObject:[NSNumber numberWithFloat:-87.54221946001053f]];
	 [testCoord addObject:[NSNumber numberWithFloat:33.21351312963314f]];
	 [testCoord addObject:[NSNumber numberWithFloat:-87.54263252019882f]];	
	 [testCoord addObject:[NSNumber numberWithFloat:33.23152659381413f]];
	 [testCoord addObject:[NSNumber numberWithFloat:-87.54215240478515f]];	
	 int count=[testCoord count];	
	 for (int i=0; i<count; i++) {
	 //NSLog(@"element %d = %@", i, [testCoord objectAtIndex:i]);
	 }*/
	
	//float a = [testCoord objectAtIndex:0];
	//NSLog(@"element = %@", [testCoord objectAtIndex:1]);
	
	//get coordinates for the ROI - problem: array is returning null
	/*mapAnn = [[MapViewController alloc] init];
	 NSLog(@"count = %@", mapAnn.mapAnnotations.count);	
	 int count=[mapAnn.mapAnnotations count];	
	 for (int i=0; i<count; i++) {
	 NSLog(@"element %i = %@", i, [mapAnn.mapAnnotations objectAtIndex:i]);
	 }*/
	
	//use convertRect:toRegionFromView to convert a rectangle to a map region
	//create function to get center of chosen ROI
	
	//float t = [testCoord objectAtIndex:0];
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
	mapAnn = nil;
}

- (void)dealloc {
	[mapView release];
	[mapAnn release];
	[testCoord release];
	[socket release];
	
	[super dealloc];
}

@end
