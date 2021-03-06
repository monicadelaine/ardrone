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

double latSpan, lonSpan, centerCoordLat, centerCoordLon;
float interval = 500;
float totalDist1 = 0, totalDist2 = 0;
float lastX1=-2.0, lastY1=132.0, lastX2=-2.0, lastY2=132.0;

UIImageView *myImage1, *myImage2;
MyAnnotation *myAnn;
MyAnnotation0 *myAnn0;
MyAnnotation1 *myAnn1;
MyAnnotation2 *myAnn2;
NSString *pointStr;
CGPoint point;
NSTimeInterval mvLastTime = 0.0, tgtLastTime = 0.0, tgtLastTime1 = 0.0, tgtLastTime2 = 0.0;

int algID;
int initDist = 0, wpReach = 0, wpReach1 = 0, wpReach2 = 0;
int enlargeFlag = 0, enlargeButton = 0, waypointUAV = 0, waypointButton = 0;
int width = 64, height = 64;
int wp_cnt = 2;
BOOL connected = FALSE;

NSString * host = @"130.160.68.35";
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
	[self sendDataToServer:pointStr]; //send init data
	[self receiveDataFromServer]; //receive data
}

//send boundary coordinates to server
-(void)sendDataToServer:(NSString *)data
{
	[socket2 sendData:[data dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
	NSLog (@"Sending: %@ %f",data, [[NSDate date] timeIntervalSince1970]*1000);		
}

//receive data from server
-(void)receiveDataFromServer
{
	NSLog(@"Start receiving data from server");
	[socket receiveWithTimeout:-1 tag:0];
}

//Called when state is received
- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    //NSLog(@"didReceiveData: %i", port);
	NSUInteger len = [data length];
	//NSLog(@"bytes %i", len);
	
	if (len > 10000) {	// checks whether data is image data
		unsigned char *byteData = (unsigned char *)[data bytes];
		if (byteData[0] == 'A') {	
			[self displayImage1:byteData]; 
		} else if (byteData[0] == 'B') {		
			[self displayImage2:byteData]; 
		} 
	} else { // or uav movement data	
		int *byteData1 = (int)[data bytes];
		[self targetInView:byteData1];
		[self wpReached:byteData1];
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

//TODO: clear image and return to map view after enlarging
// displays images coming back from Webots
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
		CGRect myImageRect1 = CGRectMake(0.0f, 276.0f, 162.0f, 97.0f); 
		myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
		[myImage1 setImage:newImage1]; 
		myImage1.opaque = YES;  
		myImage1.frame = CGRectMake(0.0, 276.0, 162.0, 97.0);
		myImage1.layer.borderColor = [UIColor redColor].CGColor;
		myImage1.layer.borderWidth = 1.0;
	}
	else if (enlargeFlag == 1) {
		CGRect myImageRect1 = CGRectMake(0.0f, 0.0f, 320.0f, 380.0f); 
		myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
		[myImage1 setImage:newImage1]; 
		myImage1.opaque = YES;  
	}
	
	[self.view addSubview:myImage1];
	
	//[NSThread sleepForTimeInterval:0.01];	
	//free(rawData1);
	[myImage1 release];	
	CGImageRelease(imageRef1);	
	CGColorSpaceRelease(colorSpaceRef);	
	CGDataProviderRelease(provider1);
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
		myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
		[myImage2 setImage:newImage2]; 
		myImage2.opaque = YES;  
		myImage2.frame = CGRectMake(162.0, 276.0, 158.0, 97.0);
		myImage2.layer.borderColor = [UIColor greenColor].CGColor;
		myImage2.layer.borderWidth = 1.0;
	}
	else if (enlargeFlag == 2) {
		CGRect myImageRect2 = CGRectMake(0.0f, 0.0f, 320.0f, 380.0f); 
		UIImageView *myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
		myImage2.image = nil; //try to clear the image 
		[myImage2 setImage:newImage2]; 
		myImage2.opaque = YES;  
	}
	
	[self.view addSubview:myImage2];
	
	//[NSThread sleepForTimeInterval:0.01];	
	//free(rawData2);
	[myImage2 release];		
	CGImageRelease(imageRef2);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider2);	
}

// action for when user finds target
- (IBAction)done:(id)sender 
{
	NSTimeInterval end = [[NSDate date] timeIntervalSince1970]*1000;

	NSLog(@"totalDist1=%f totalDist2=%f", totalDist1, totalDist2);	
	NSLog(@"Done %f", end);
	
	wpReach = 0, wpReach1 = 0, wpReach2 = 0, wp_cnt = 2;
	connected = FALSE;
	
	[socket close];
	[socket2 close];
	[socket release];
	[socket2 release];	
}

// clears the uav waypoint pins in the roi
- (IBAction)roi:(id)sender 
{
	NSLog(@"Clear ROI button");
	//self.mapView.hidden = NO; //try to show map again
	enlargeFlag = 0;
	
	for (id annotation in mapView.annotations) {   
		if ([annotation isKindOfClass:[MyAnnotation class]] ) {    			
			[mapView removeAnnotation:annotation];     
		} else if ([annotation isKindOfClass:[MyAnnotation0 class]]) {    			
			[mapView removeAnnotation:annotation];     
		}    	
	}	
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

// allows user to choose a waypoint for a uav
- (IBAction)waypoint:(id)sender 
{
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

// add pin for user waypoint 
- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	if (sender.state==UIGestureRecognizerStateBegan) {	
		//action for Waypoint 
		NSLog(@"Waypoint");
		if (algID == 1 || algID == 3) {
			if (self.mapView.hidden == YES) {
				self.mapView.hidden = NO; //show map again
			}
			waypointButton=1;
			alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Choose waypoint for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
			[alertButton show];
			[alertButton release];	
		
			point = [sender locationInView:self.mapView];
		}
	}	
} 

// sends waypoint to server
- (void) addWaypoint
{
	myAnn = [[MyAnnotation alloc] init];	
	myAnn0 = [[MyAnnotation0 alloc] init];	
	[waypoint removeAllObjects];

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
	
	[self.pointWaypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:wpReach];	
	[self.pointWaypoint insertObject:[NSNumber numberWithDouble:p.x-MAPPOINTX] atIndex:wpReach+1];	
	[self.pointWaypoint insertObject:[NSNumber numberWithDouble:p.y-MAPPOINTY] atIndex:wpReach+2];	
	wpReach+=3;
	
	NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
	
	//send waypoint to server
	NSString *waypointStr = [NSString stringWithFormat:@"1,%i,%f,%f,%i",waypointUAV,p.x-MAPPOINTX,p.y-MAPPOINTY,wp_cnt];		
	[socket2 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
	NSLog (@"Sending waypoint: %@ %f",waypointStr, wpTime);
	wp_cnt++;
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
				[self addWaypoint];
			}
		} else {
			NSLog(@"UAV2 button pressed\n");
			if (enlargeButton==1) {
				enlargeFlag = 2;
			} else if (waypointButton==1) {
				waypointUAV = 2;
				[self addWaypoint];
			}
		}
		enlargeButton=0;
		waypointButton=0;
	}
	
	//for testing
	if (alertView == alertButtonTest) {
			NSLog(@"Target in View");
	}
}

// determines which pin and uav to display
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

// moves uav according to data received from Webots
- (void)moveUAVs:(NSMutableArray *)data
{	
	MKMapPoint p1 = MKMapPointMake([[data objectAtIndex:0] doubleValue],[[data objectAtIndex:1]doubleValue]);
	MKMapPoint p2 = MKMapPointMake([[data objectAtIndex:2] doubleValue],[[data objectAtIndex:3]doubleValue]);
	p1.x += MAPPOINTX; p2.x += MAPPOINTX;
	p1.y += MAPPOINTY; p2.y += MAPPOINTY;
	CLLocationCoordinate2D uavCoord1 = MKCoordinateForMapPoint(p1);
	CLLocationCoordinate2D uavCoord2 = MKCoordinateForMapPoint(p2);														   
	
	NSTimeInterval currTime = [[NSDate date] timeIntervalSince1970]*1000;
	if (currTime > mvLastTime+interval) {
		//NSLog(@"moveUAVs: p1=%f,%f p2=%f,%f %f", p1.x-MAPPOINTX, p1.y-MAPPOINTY, p2.x-MAPPOINTX, p2.y-MAPPOINTY, currTime);
		mvLastTime = [[NSDate date] timeIntervalSince1970]*1000;		
	
		[self.mapView removeAnnotation:myAnn1]; 
		[self.mapView removeAnnotation:myAnn2]; 
	
		/*MyAnnotation1 **/myAnn1 = [[MyAnnotation1 alloc] init];
		myAnn1.latitude = [NSNumber numberWithDouble:uavCoord1.latitude];
		myAnn1.longitude = [NSNumber numberWithDouble:uavCoord1.longitude];	
		[self.mapView addAnnotation:myAnn1];
		[myAnn1 release];	
	
		/*MyAnnotation2 **/myAnn2 = [[MyAnnotation2 alloc] init];
		myAnn2.latitude = [NSNumber numberWithDouble:uavCoord2.latitude];
		myAnn2.longitude = [NSNumber numberWithDouble:uavCoord2.longitude];	
		[self.mapView addAnnotation:myAnn2];
		[myAnn2 release];
	}
}

// determine if uav reached waypoint
- (void)wpReached:(int *)data
{
	int threshold = 5;
	//uav1
	if (wpReach1 < wpReach) {
		double a = [[pointWaypoint objectAtIndex:wpReach1] doubleValue];
		if (a == 1) {
			double b = [[pointWaypoint objectAtIndex:wpReach1+1] doubleValue];
			double c = [[pointWaypoint objectAtIndex:wpReach1+2] doubleValue];
			float x1 = pow(data[0] - b, 2.0);
			float y1 = pow(data[1] - c, 2.0);
			float dist = sqrt((float)(x1+y1));	
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav1 waypoint %f %f reached %f", b, c, wpTime);
				wpReach1 +=3;			
			}
		} else if (a == 2 && wpReach1 < wpReach) {
			wpReach1 += 3;
		}
	}
	
	//uav2
	if (wpReach2 < wpReach) {
		double a = [[pointWaypoint objectAtIndex:wpReach2] doubleValue];
		if (a == 2) {
			double b = [[pointWaypoint objectAtIndex:wpReach2+1] doubleValue];
			double c = [[pointWaypoint objectAtIndex:wpReach2+2] doubleValue];
			float x1 = pow(data[2] - b, 2.0);
			float y1 = pow(data[3] - c, 2.0);
			float dist = sqrt((float)(x1+y1));		
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav2 waypoint %f %f reached %f", b, c, wpTime);
				wpReach2 +=3;
			}
		} else if (a == 1 && wpReach2 < wpReach) {
			wpReach2 += 3;
		}
	}
}

//TODO: find out correct dist of cam view
// checks if target is in view
- (void)targetInView:(int *)data
{
	int x1, y1, x2, y2;
	int threshold = 65;
	float dist1, dist2;
	
	NSTimeInterval tgtTime = [[NSDate date] timeIntervalSince1970]*1000;
		
	// uav1
	if (data[0] != 0 && data[1] != 0) {
		x1 = pow(data[0] - data[4], 2.0);
		y1 = pow(data[1] - data[5], 2.0);
		dist1 = sqrt((float)(x1+y1));
		if (initDist == 0) {
			NSLog(@"init dist between uav1 (%i,%i) and target (%i,%i): %f", data[0], data[1], data [4], data[5], dist1);	
		}	
		if (dist1 < threshold) { // find out dist of cam view
			if (tgtTime > tgtLastTime1+interval) {
				NSLog(@"Target in UAV 1 view dist: %f %f", dist1, tgtTime);
				tgtLastTime1 = [[NSDate date] timeIntervalSince1970]*1000;	
				//for testing
				/*alertButtonTest = [[UIAlertView alloc] initWithTitle:@"" message:@"Target in UAV1 view" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
				[alertButtonTest show];
				[alertButtonTest release];*/			
			}
		}
	}								  
					
	// uav2
	if (data[2] != 0 && data[3] != 0) {	
		x2 = pow(data[2] - data[4], 2.0);
		y2 = pow(data[3] - data[5], 2.0);
		dist2 = sqrt((float)(x2+y2));	
		if (initDist == 0) {
			NSLog(@"init dist between uav2 (%i,%i) and target(%i,%i): %f", data[2], data[3], data [4], data[5], dist2);		
			initDist = 1;
		}	
		if (dist2 < threshold) { // find out dist of cam view
			if (tgtTime > tgtLastTime2+interval) {
				NSLog(@"Target in UAV 2 view dist: %f %f", dist2, tgtTime);
				tgtLastTime2 = [[NSDate date] timeIntervalSince1970]*1000;		
				//for testing
				/*alertButtonTest = [[UIAlertView alloc] initWithTitle:@"" message:@"Target in UAV2 view" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
				[alertButtonTest show];
				[alertButtonTest release];	*/			
			}
		}
	}			
	
	if ((tgtTime > tgtLastTime+interval) && (data[0] != 0) && (data[2] != 0)) {
		NSLog(@"p1=%i,%i p2=%i,%i target=%i,%i dist1=%f dist2=%f %f", data[0], data[1], data [2], data[3], data[4], data[5], dist1, dist2, tgtTime);		
		tgtLastTime = [[NSDate date] timeIntervalSince1970]*1000;	
	
		int dx1 = pow(data[0] - lastX1, 2.0);
		int dy1 = pow(data[1] - lastY1, 2.0);
		totalDist1 += sqrt((float)(dx1+dy1));
	
		int dx2 = pow(data[2] - lastX2, 2.0);	
		int dy2 = pow(data[3] - lastY2, 2.0);
		totalDist2 += sqrt((float)(dx2+dy2));
	
		lastX1 = data[0], lastY1 = data[1];
		lastX2 = data[2], lastY2 = data[3];	
	}
}

// get the coordinates passed from MapViewController
- (void) setPoint:(NSMutableArray *)passedCoord:(int)passedAlg:(NSString *)passedStr
{
	initCoord = passedCoord;
	algID = passedAlg;
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

// set up roi on map
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
		//NSLog(@"ROI params: latSpan=%f lonSpan=%f centerLat=%f centerLon=%f", latSpan, lonSpan, centerCoordLat, centerCoordLon);
		coord.latitude=centerCoordLat;
		coord.longitude=centerCoordLon;
		region.center=coord;
		MKCoordinateSpan span = {latSpan, lonSpan};
		region.span = span;
	}
	[mapView setRegion:region animated:YES];
}
	
- (void) initVar
{
	mvLastTime = 0.0, tgtLastTime1 = 0.0, tgtLastTime2 = 0.0;
	initDist = 0, wpReach = 0, wpReach1 = 0, wpReach2 = 0;
	waypointUAV = 0, waypointButton = 0;
}

- (void)viewDidLoad
{
	NSTimeInterval start = [[NSDate date] timeIntervalSince1970]*1000;
	NSLog(@"Start %f", start);
	
	[self initVar];
	mapView.delegate=self;
	
	[self regionSet]; 
	[self setMap];	
		
	[self connectToServer];
	
	self.waypoint = [[NSMutableArray alloc] init];
	self.pointWaypoint = [[NSMutableArray alloc] init];	
	
	UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
	[self.mapView addGestureRecognizer:longPressGesture];
	[longPressGesture release]; 	
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload
{
	self.mapView = nil;
	self.initCoord = nil;
	self.waypoint = nil;
	self.pointWaypoint = nil;
}

- (void)dealloc {
	[mapView release];
	[initCoord release];
	[waypoint release];
	[pointWaypoint release];
	
	[super dealloc];
}

	
@end
