//
//  RoiViewController.m

#import <QuartzCore/QuartzCore.h>

#import "RoiViewController.h"
#import "MapViewController.h"
#import "MyAnnotation.h"

#define MAPPOINTX 68940600
#define MAPPOINTY 107935300

enum {
    kAnnotationIndex = 0,
};	


@implementation RoiViewController

@synthesize mapView, initCoord, waypoint, pointWaypoint;
@synthesize socket, socket2;
@synthesize imageView;

double latSpan, lonSpan, centerCoordLat, centerCoordLon;

UIImageView *myImage1, *myImage2;
MyAnnotation *myAnn;
MyAnnotation0 *myAnn0;
NSString *pointStr;

int enlargeFlag = 0, enlargeButton = 0, waypointUAV = 0, waypointButton = 0;
int width = 64, height = 64;
int wp_cnt = 2;
BOOL connected = FALSE;

NSString * host = @"130.160.68.24";
UInt16 iport = 1501;
UInt16 oport = 1506;

//initialize sockets and connect to server
-(void)connectToServer
{
	if (connected == FALSE) {
		NSLog(@"Connecting To Server");
		
		socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
		socket2 = [[AsyncUdpSocket alloc] initWithDelegate:self];
		NSError *error = nil;
	
		if ([socket connectToHost:host onPort:iport error:&error]) { // connect to socket
			NSLog(@"Connected on port %i", iport);
		} else {
			NSLog(@"Error connecting to server at %i: %@", iport, error);
		}
		
		NSString *data = @"Sending to initialize receive";
		[socket sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
		NSLog (@"%@",data);
		
		connected = TRUE;
	}		
	[self sendDataToServer:pointStr]; //send data
	[self receiveDataFromServer]; //receive data
}

//send boundary coordinates to server
-(void)sendDataToServer:(NSString *)data
{
	if ([initCoord count]!=0) {	
		[socket2 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending: %@",data);	
	} else {
		NSString *data = @"Default ROI";
		[socket2 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending: %@",data);	
	}
}

//receive data from server
-(void)receiveDataFromServer
{
	NSLog(@"Receiving Data From Server");
	[socket receiveWithTimeout:-1 tag:0];
}

//Called when the datagram has been sent
- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag 
{
	NSLog(@"didSendData");
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
	NSLog(@"didNotSendData: %@", error);
}

//Called when state is received
- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    //NSLog(@"didReceiveData: %i", port);
	NSUInteger len = [data length];
	//NSLog(@"bytes %i", len);
	
	if (len > 10000) {	
		unsigned char *byteData = (unsigned char *)[data bytes];
		if (byteData[0] == 'A') {	
			[self displayImage1:byteData]; 
		} else if (byteData[0] == 'B') {		
			[self displayImage2:byteData]; 
		} 
	} else {	
		int *byteData1 = (int)[data bytes];
		NSMutableArray *posData = [[NSMutableArray alloc] init];
		[posData insertObject:[NSNumber numberWithDouble:byteData1[3]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData1[2]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData1[1]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData1[0]]atIndex:kAnnotationIndex];		
		[self moveUAVs:posData]; 	
	}
	[sock receiveWithTimeout:-1 tag:0]; 
	return YES;
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotReceiveDataWithTag:(long)tag dueToError:(NSError *)error {
	NSLog(@"didNotReceiveData: %@", error);
}

//TODO: clear image and return to map view after enlarging
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
	
	if (enlargeFlag == 0) {
		//NSLog(@"width %i height %i", self.frame.size.width=320, self.frame.size.height=480);
		CGRect myImageRect1 = CGRectMake(0.0f, 276.0f, 162.0f, 97.0f); 
		/*UIImageView **/myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
		[myImage1 setImage:newImage1]; 
		myImage1.opaque = YES;  
		myImage1.frame = CGRectMake(0.0, 276.0, 162.0, 97.0);
		myImage1.layer.borderColor = [UIColor redColor].CGColor;
		myImage1.layer.borderWidth = 1.0;
	}
	else if (enlargeFlag == 1) {
		//self.imageView.hidden = YES;
		//myImage1.image = nil; //try to clear the image 
		CGRect myImageRect1 = CGRectMake(0.0f, 0.0f, 320.0f, 380.0f); 
		/*UIImageView **/myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
		[myImage1 setImage:newImage1]; 
		myImage1.opaque = YES;  
	}
	
	[self.view addSubview:myImage1];
	[myImage1 release];	
}

- (void) displayImage2:(unsigned char *)data
{
	unsigned char *rawData2 = malloc(width*height*4);
	
	for (int i=0; i<width*height; ++i) {
		rawData2[4*i] = data[3*i];
		rawData2[4*i+1] = data[3*i+1];
		rawData2[4*i+2] = data[3*i+2];
		rawData2[4*i+3] = 255; 
	}
	
	CGDataProviderRef provider2 = CGDataProviderCreateWithData(NULL,rawData2,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef2 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider2,NULL,NO,renderingIntent);	
	UIImage *newImage2 = [UIImage imageWithCGImage:imageRef2];	
	
	if (enlargeFlag == 0) {
		CGRect myImageRect2 = CGRectMake(162.0f, 276.0f, 158.0f, 97.0f);
		/*UIImageView **/myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
		[myImage2 setImage:newImage2]; 
		myImage2.opaque = YES;  
		myImage2.frame = CGRectMake(162.0, 276.0, 158.0, 97.0);
		myImage2.layer.borderColor = [UIColor greenColor].CGColor;
		myImage2.layer.borderWidth = 1.0;
	}
	else if (enlargeFlag == 2) {
		//self.imageView.hidden = YES;
		//myImage2.image = nil; //try to clear the image 
		CGRect myImageRect2 = CGRectMake(0.0f, 0.0f, 320.0f, 380.0f); 
		//self.imageView.hidden = YES;
		/*UIImageView **/myImage1 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
		myImage2.image = nil; //try to clear the image 
		[myImage2 setImage:newImage2]; 
		myImage2.opaque = YES;  
	}
	
	[self.view addSubview:myImage2];
	//myImage2.image = nil; //try to clear the image
	[myImage2 release];		
}

//TODO: action for when user finds target and restarts app
- (IBAction)done:(id)sender 
{
	NSLog(@"Done button");
	[socket close];
	[socket2 close];
}

- (IBAction)roi:(id)sender 
{
	//action for ROI
	NSLog(@"ROI button");
	self.imageView.hidden = YES; //try to clear the image  
	self.mapView.hidden = NO; //try to show map again
	[self.mapView removeAnnotations:mapView.annotations];
	enlargeFlag = 0;
}

- (IBAction)enlarge:(id)sender 
{	
	//action for Enlarge
	NSLog(@"Enlarge button");
	enlargeButton=1;
	self.mapView.hidden = YES;	//try to hide map	
	alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Enlarge video for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
	[alertButton show];
	[alertButton release];		
}

- (IBAction)waypoint:(id)sender 
{
	//action for Waypoint 
	NSLog(@"Waypoint button");
	if (self.mapView.hidden == YES) {
		self.mapView.hidden = NO; //show map again
	}
	waypointButton=1;
	alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Choose waypoint for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
	[alertButton show];
	[alertButton release];
			
	UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
	[self.mapView addGestureRecognizer:longPressGesture];
	[longPressGesture release];  			
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if(alertView == alertButton) {
		if(buttonIndex == 0) {
			NSLog(@"UAV1 button pressed\n");
			if (enlargeButton==1) {
				enlargeFlag = 1;
			} else if (waypointButton==1) {
				waypointUAV = 1;
			}
		} else {
			NSLog(@"UAV2 button pressed\n");
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

- (MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{ 
	if ([annotation isKindOfClass:[MyAnnotation class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		pinView.canShowCallout = YES;		
		return pinView;
	} else if ([annotation isKindOfClass:[MyAnnotation0 class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		pinView.pinColor = MKPinAnnotationColorGreen;
		pinView.canShowCallout = YES;		
		return pinView;
	} else if ([annotation isKindOfClass:[MyAnnotation1 class]]) {
		MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"UAV1"] autorelease];
		UIImage *uavImage = [UIImage imageNamed:@"uav1.png"];
		uavView.canShowCallout = YES;		
		uavView.image=uavImage;
		uavView.opaque = YES;  
		[uavImage release];	
		return uavView;
	} else if ([annotation isKindOfClass:[MyAnnotation2 class]]) {
		MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"UAV2"] autorelease];
		UIImage *uavImage = [UIImage imageNamed:@"uav2_.png"];
		uavView.canShowCallout = YES;			
		uavView.image=uavImage;
		uavView.opaque = YES;  
		[uavImage release];	
		return uavView;	
	}
	return nil;
}

//TODO: clear annotation/path for old uav position
- (void)moveUAVs:(NSMutableArray *)data
{
	/*for (int i=0; i<4; i++) {
		NSLog(@"uav pos %i = %@", i, [data objectAtIndex:i]);
	}*/
	
	MKMapPoint p1 = MKMapPointMake([[data objectAtIndex:0] doubleValue],[[data objectAtIndex:1]doubleValue]);
	MKMapPoint p2 = MKMapPointMake([[data objectAtIndex:2] doubleValue],[[data objectAtIndex:3]doubleValue]);
	p1.x += MAPPOINTX; p2.x += MAPPOINTX;
	p1.y += MAPPOINTY; p2.y += MAPPOINTY;
	CLLocationCoordinate2D uavCoord1 = MKCoordinateForMapPoint(p1);
	CLLocationCoordinate2D uavCoord2 = MKCoordinateForMapPoint(p2);														   
	
	NSLog(@"moveUAVs map point: p1=%f,%f p2=%f,%f", p1.x, p1.y, p2.x, p2.y);
	
	//[mapView removeAnnotations:mapView.annotations]; 
	
	MyAnnotation1 *myAnn1 = [[MyAnnotation1 alloc] init];
	myAnn1.latitude = [NSNumber numberWithDouble:uavCoord1.latitude];
	myAnn1.longitude = [NSNumber numberWithDouble:uavCoord1.longitude];	
	[self.mapView addAnnotation:myAnn1];
	[myAnn1 release];	
	
	MyAnnotation2 *myAnn2 = [[MyAnnotation2 alloc] init];
	myAnn2.latitude = [NSNumber numberWithDouble:uavCoord2.latitude];
	myAnn2.longitude = [NSNumber numberWithDouble:uavCoord2.longitude];	
	[self.mapView addAnnotation:myAnn2];
	[myAnn2 release];
	
	NSLog(@"moveUAVs coord: p1=%@,%@ p2=%@,%@", myAnn1.latitude, myAnn1.longitude, myAnn2.latitude, myAnn2.longitude);
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	myAnn = [[MyAnnotation alloc] init];	
	myAnn0 = [[MyAnnotation0 alloc] init];	
	[waypoint removeAllObjects];
	[pointWaypoint removeAllObjects];	
	if (sender.state==UIGestureRecognizerStateBegan) {
		CGPoint point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.longitude]atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.latitude] atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];
		
		MKMapPoint p = MKMapPointForCoordinate(locCoord);
		
		if (waypointUAV==1) {
			myAnn.latitude = [NSNumber numberWithDouble:locCoord.latitude];
			myAnn.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
			[self.mapView addAnnotation:myAnn];
			[myAnn release];
		} else if (waypointUAV==2) {
			myAnn0.latitude = [NSNumber numberWithDouble:locCoord.latitude];
			myAnn0.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
			[self.mapView addAnnotation:myAnn0];
			[myAnn0 release];
		}			
		
		[self.pointWaypoint insertObject:[NSNumber numberWithDouble:p.y] atIndex:kAnnotationIndex];	
		[self.pointWaypoint insertObject: [NSNumber numberWithDouble:p.x] atIndex:kAnnotationIndex];	
		[self.pointWaypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];		
		
		int count=[waypoint count];
		for (int i=0; i<count; i++) {
			NSLog(@"waypoint coord %i = %@", i, [waypoint objectAtIndex:i]);
		}
		int pcnt=[pointWaypoint count];
		for (int i=0; i<pcnt; i++) {
			NSLog(@"waypoint map point %i = %@", i, [pointWaypoint objectAtIndex:i]);
		}

		//send waypoint to server
		NSString *waypointStr = [NSString stringWithFormat:@"%i,%f,%f,%i",waypointUAV,p.x-MAPPOINTX,p.y-MAPPOINTY,wp_cnt];		
		[socket2 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending: %@",waypointStr);
		wp_cnt++;
	}	
} 

- (void) setPoint:(NSMutableArray *)passedCoord:(NSMutableArray *)passedPoint:(NSString *)passedStr
{
	initCoord = passedCoord;
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
	if ([initCoord count]!=0) {	
		latSpan = [self calcDist:0:1:6:7];
		lonSpan = [self calcDist:0:1:2:3];
		centerCoordLat = [self centerPt:6:2];
		centerCoordLon = [self centerPt:7:3];
	}
}

- (void) setMap
{
	mapView.mapType = MKMapTypeSatellite;

	CLLocationCoordinate2D coord;
	MKCoordinateRegion region;
	if ([initCoord count]==0) {
		//use default ROI
		coord.latitude=33.2131;
		coord.longitude=-87.5426;	
		region.center=coord;
		MKCoordinateSpan span = {0.001, 0.001};
		region.span = span;
	} else {	
		//uses coordinates chosen by user to display ROI
		NSLog(@"ROI params: latSpan=%f lonSpan=%f centerLat=%f centerLon=%f", latSpan, lonSpan, centerCoordLat, centerCoordLon);
		coord.latitude=centerCoordLat;
		coord.longitude=centerCoordLon;
		region.center=coord;
		MKCoordinateSpan span = {latSpan, lonSpan};
		region.span = span;
	}
	[mapView setRegion:region animated:YES];
}
	
- (void)viewDidLoad
{
	//set ROI region
	mapView.delegate=self;
	[self regionSet]; 
	[self setMap];	
		
	[self connectToServer];
	
	self.waypoint = [[NSMutableArray alloc] init];
	self.pointWaypoint = [[NSMutableArray alloc] init];	
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
	//[socket release];
	//[socket2 release];	
	
	[super dealloc];
}

	
@end
