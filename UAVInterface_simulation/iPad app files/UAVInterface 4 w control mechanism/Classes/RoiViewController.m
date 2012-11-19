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

@synthesize mapView, initCoord, waypoint, pointWaypoint, socket, socket2;

double latSpan, lonSpan, centerCoordLat, centerCoordLon;
float interval = 500;
float totalDist1 = 0, totalDist2 = 0, totalDist3 = 0, totalDist4 = 0;
int lastX1=-2, lastY1=132, lastX2=-2, lastY2=132, lastX3=-2, lastY3=132, lastX4=-2, lastY4=132;

UIImageView *myImage1, *myImage2, *myImage3, *myImage4;
MyAnnotation *myAnn;   MyAnnotation0 *myAnn0; MyAnnotation3 *myAnn3; MyAnnotation5 *myAnn5; //wp
MyAnnotation1 *myAnn1; MyAnnotation2 *myAnn2; MyAnnotation4 *myAnn4; MyAnnotation6 *myAnn6; //uav
MKPolyline *polyl, *polyl2, *polyl3, *polyl4; //route
MKPolygon *poly, *poly2, *poly3, *poly4; //overlay
NSMutableArray *allpoly;

NSString *pointStr;
CGPoint point;
NSTimeInterval overLastTime=0, mvLastTime=0, tgtLastTime=0, tgtLastTime1=0, tgtLastTime2=0, tgtLastTime3=0, tgtLastTime4=0, startMove=0;

int algID, numUAV, d=0, st=0;
int initDist = 0, wpReach = 0, wpReach1 = 0, wpReach2 = 0, wpReach3 = 0, wpReach4 = 0;
int waypointUAV = 0, wp_cnt = 2;
int width = 64, height = 64;
BOOL connected = FALSE;
unsigned char *rawData1, *rawData2, *rawData3, *rawData4;

NSString * host = @"130.160.47.198";
UInt16 iport = 1501, oport = 1506;


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
	NSUInteger len = [data length];
	//NSLog(@"bytes %i", len);
	
	if (len > 10000) {	// checks whether data is image data
		unsigned char *byteData = (unsigned char *)[data bytes];
		if (byteData[0] == 'A' && byteData[1] == 'A') {	
			[self displayImage1:byteData]; 
		} else if (byteData[0] == 'B') {		
			[self displayImage2:byteData]; 
		} else if (byteData[0] == 'C') {	
			[self displayImage3:byteData];
		} else if (byteData[0] == 'D') {	
			[self displayImage4:byteData];
		}
	} else { // or uav movement data	
		int *byteData1 = (int)[data bytes];
		if (byteData1[18] == -2) {
			[self targetInView:byteData1];
			[self wpReached:byteData1];
			[self moveUAVs:byteData1];
			[self addRouteLine:byteData1];
		} else {
			[self addOverlayArea:byteData1]; 
		}
	}
	[sock receiveWithTimeout:-1 tag:0]; 
	return YES;
}

// displays images coming back from Webots
- (void) displayImage1:(unsigned char *)data
{
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
	
	float i = 320/numUAV;
	CGRect myImageRect1 = CGRectMake(i*0, 276.0, i, 97.0); 
	myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
	[myImage1 setImage:newImage1]; 
	myImage1.opaque = YES;  
	myImage1.frame = CGRectMake(i*0, 276.0, i, 97.0);
	myImage1.layer.borderColor = [UIColor redColor].CGColor;
	myImage1.layer.borderWidth = 1.0;
	
	[self.view addSubview:myImage1];
	
	[myImage1 release];	
	CGImageRelease(imageRef1);	
	CGColorSpaceRelease(colorSpaceRef);	
	CGDataProviderRelease(provider1);
}

- (void) displayImage2:(unsigned char *)data
{	
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
	
	float i = 320/numUAV;
	CGRect myImageRect2 = CGRectMake(i*1, 276.0, i, 97.0);
	myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
	[myImage2 setImage:newImage2]; 
	myImage2.opaque = YES;  
	myImage2.frame = CGRectMake(i*1, 276.0, i, 97.0);
	myImage2.layer.borderColor = [UIColor greenColor].CGColor;
	myImage2.layer.borderWidth = 1.0;
	
	[self.view addSubview:myImage2];

	[myImage2 release];		
	CGImageRelease(imageRef2);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider2);	
}

- (void) displayImage3:(unsigned char *)data 
{	
	for (int i=0; i<width*height; ++i) {
		rawData3[4*i] = data[3*i];
		rawData3[4*i+1] = data[3*i+1];
		rawData3[4*i+2] = data[3*i+2];
		rawData3[4*i+3] = 255; 
	}
	
	CGDataProviderRef provider3 = CGDataProviderCreateWithData(NULL,rawData3,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef3 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider3,NULL,NO,renderingIntent);	
	UIImage *newImage3 = [UIImage imageWithCGImage:imageRef3];	
	
	float i = 320/numUAV;
	CGRect myImageRect3 = CGRectMake(i*2, 276.0, i, 97.0);
	myImage3 = [[UIImageView alloc] initWithFrame:myImageRect3]; 
	[myImage3 setImage:newImage3]; 
	myImage3.opaque = YES;  
	myImage3.frame = CGRectMake(i*2, 276.0, i, 97.0);
	myImage3.layer.borderColor = [UIColor purpleColor].CGColor;
	myImage3.layer.borderWidth = 1.0;
	
	[self.view addSubview:myImage3];

	[myImage3 release];		
	CGImageRelease(imageRef3);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider3);	
}

- (void) displayImage4:(unsigned char *)data 
{	
	for (int i=0; i<width*height; ++i) {
		rawData4[4*i] = data[3*i];
		rawData4[4*i+1] = data[3*i+1];
		rawData4[4*i+2] = data[3*i+2];
		rawData4[4*i+3] = 255; 
	}
	
	CGDataProviderRef provider4 = CGDataProviderCreateWithData(NULL,rawData4,width*height*4,NULL);
	
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4*width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef imageRef4 = CGImageCreate(width,height,bitsPerComponent,bitsPerPixel,bytesPerRow,colorSpaceRef,bitmapInfo,provider4,NULL,NO,renderingIntent);	
	UIImage *newImage4 = [UIImage imageWithCGImage:imageRef4];	
	
	float i = 320/numUAV;
	CGRect myImageRect4 = CGRectMake(i*3, 276.0, i, 97.0);
	myImage4 = [[UIImageView alloc] initWithFrame:myImageRect4]; 
	[myImage4 setImage:newImage4]; 
	myImage4.opaque = YES;  
	myImage4.frame = CGRectMake(i*3, 276.0, i, 97.0);
	myImage4.layer.borderColor = [UIColor blueColor].CGColor;
	myImage4.layer.borderWidth = 1.0;
	
	[self.view addSubview:myImage4];

	[myImage4 release];		
	CGImageRelease(imageRef4);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider4);	
}

// action for when user finds target
- (IBAction)done:(id)sender 
{
	NSTimeInterval end = [[NSDate date] timeIntervalSince1970]*1000;

	int pcnt=[pointWaypoint count], cnt=1;
	 for (int i=0; i<pcnt; i+=5,cnt++) {
	 NSLog(@"waypoint %i = %@,%@,%@,%@,%@", cnt,[pointWaypoint objectAtIndex:i],[pointWaypoint objectAtIndex:i+1],[pointWaypoint objectAtIndex:i+2],
		   [pointWaypoint objectAtIndex:i+3],[pointWaypoint objectAtIndex:i+4]);
	 }
	
	NSLog(@"totalDist1=%f totalDist2=%f totalDist3=%f totalDist4=%f", totalDist1, totalDist2, totalDist3, totalDist4);	
	NSLog(@"Total time moving %f", end-startMove);
	NSLog(@"Done %f", end);
	
	alertButtonEnd = [[UIAlertView alloc] initWithTitle:@"" message:@"Surveillance ended!" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertButtonEnd show];
	[alertButtonEnd release];	
	
	wpReach = 0, wpReach1 = 0, wpReach2 = 0, wpReach3 = 0, wpReach4 = 0, wp_cnt = 2;
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
	for (id annotation in mapView.annotations) {   
		if ([annotation isKindOfClass:[MyAnnotation class]] ) {    			
			[mapView removeAnnotation:annotation];     
		} else if ([annotation isKindOfClass:[MyAnnotation0 class]]) {    			
			[mapView removeAnnotation:annotation];     
		} else if ([annotation isKindOfClass:[MyAnnotation3 class]]) {    			
			[mapView removeAnnotation:annotation];     
		} else if ([annotation isKindOfClass:[MyAnnotation5 class]]) {    			
			[mapView removeAnnotation:annotation];     
		}	
	}	
}

// add pin for user waypoint 
- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	if (sender.state==UIGestureRecognizerStateBegan) {	
		//action for Waypoint 
		NSLog(@"Waypoint");
		if (algID == 1 || algID == 3) {
			if (numUAV == 2) {
				alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Choose waypoint for" delegate:self cancelButtonTitle:nil otherButtonTitles:@"UAV1",@"UAV2", nil];
			} else if (numUAV == 3) {
				alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Choose waypoint for" delegate:self cancelButtonTitle:nil otherButtonTitles:@"UAV1",@"UAV2",@"UAV3", nil];
			} else if (numUAV == 4) {
				alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Choose waypoint for" delegate:self cancelButtonTitle:nil otherButtonTitles:@"UAV1",@"UAV2",@"UAV3",@"UAV4", nil];
				
				UIImageView *imageView1 = [[UIImageView alloc] initWithFrame:CGRectMake(207.0, 73.0, 13.0, 13.0)];
				UIImage *img1 = [UIImage imageNamed:@"uav1.png"];
				[imageView1 setImage:img1];
				[alertButton addSubview:imageView1];
				[imageView1 release];
				
				UIImageView *imageView2 = [[UIImageView alloc] initWithFrame:CGRectMake(207.0, 122.0, 13.0, 13.0)];
				UIImage *img2 = [UIImage imageNamed:@"uav2_.png"];
				[imageView2 setImage:img2];
				[alertButton addSubview:imageView2];
				[imageView2 release];
				
				UIImageView *imageView3 = [[UIImageView alloc] initWithFrame:CGRectMake(207.0, 173.0, 13.0, 13.0)];
				UIImage *img3 = [UIImage imageNamed:@"uav3.png"];
				[imageView3 setImage:img3];
				[alertButton addSubview:imageView3];
				[imageView3 release];
				
				UIImageView *imageView4 = [[UIImageView alloc] initWithFrame:CGRectMake(207.0, 237.0, 13.0, 13.0)];
				UIImage *img4 = [UIImage imageNamed:@"uav4.png"];
				[imageView4 setImage:img4];
				[alertButton addSubview:imageView4];
				[imageView4 release];
			}
			[alertButton show];
			[alertButton release];	
			
			point = [sender locationInView:self.mapView];
		}
	}	
} 

// sends waypoint to server
- (void) addWaypoint
{
	[waypoint removeAllObjects];

	CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
	[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.longitude]atIndex:kAnnotationIndex];
	[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.latitude] atIndex:kAnnotationIndex];
	[self.waypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];
	
	MKMapPoint p = MKMapPointForCoordinate(locCoord);
	
	if (waypointUAV==1) {
		myAnn = [[MyAnnotation alloc] init];
		myAnn.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnn.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
		[self.mapView addAnnotation:myAnn];
		[myAnn release];
	} else if (waypointUAV==2) {
		myAnn0 = [[MyAnnotation0 alloc] init];	
		myAnn0.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnn0.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
		[self.mapView addAnnotation:myAnn0];
		[myAnn0 release];
	} else if (waypointUAV==3) {
		myAnn3 = [[MyAnnotation3 alloc] init];
		myAnn3.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnn3.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
		[self.mapView addAnnotation:myAnn3];
		[myAnn3 release];
	} else if (waypointUAV==4) {
		myAnn5 = [[MyAnnotation5 alloc] init];
		myAnn5.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnn5.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
		[self.mapView addAnnotation:myAnn5];
		[myAnn5 release];
	}		
	
	NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
	
	[self.pointWaypoint insertObject:[NSNumber numberWithInt:waypointUAV] atIndex:wpReach];	
	[self.pointWaypoint insertObject:[NSNumber numberWithInt:p.x-MAPPOINTX] atIndex:wpReach+1];	
	[self.pointWaypoint insertObject:[NSNumber numberWithInt:p.y-MAPPOINTY] atIndex:wpReach+2];	
	[self.pointWaypoint insertObject:[NSNumber numberWithDouble:wpTime] atIndex:wpReach+3];
	[self.pointWaypoint insertObject:[NSNumber numberWithDouble:0] atIndex:wpReach+4];
	wpReach+=5;
	
	if (d==1 && algID==1) { //send dummy value to server
		NSString *waypointStr = [NSString stringWithFormat:@"1,1,0,200,1"];		
		[socket2 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending dummy waypoint1: %@ %i",waypointStr, algID);
		d=2;
	}
	
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
			waypointUAV = 1;
			[self addWaypoint];
		} else if(buttonIndex == 1) {
			NSLog(@"UAV2 button pressed\n");
			waypointUAV = 2;
			[self addWaypoint];
		} else if(buttonIndex == 2) {
			NSLog(@"UAV3 button pressed\n");
			waypointUAV = 3;
			[self addWaypoint];
		} else {
			NSLog(@"UAV4 button pressed\n");
			waypointUAV = 4;
			[self addWaypoint];
		}
	} else if (alertView == alertButtonEnd) {
		NSLog(@"Surveillance end alert");
	}
}

// determines which pin and uav to display
- (MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{ 
	if ([annotation isKindOfClass:[MyAnnotation class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		return pinView;
	} else if ([annotation isKindOfClass:[MyAnnotation0 class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		pinView.pinColor = MKPinAnnotationColorGreen;		
		return pinView;
	} else if ([annotation isKindOfClass:[MyAnnotation3 class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		pinView.pinColor = MKPinAnnotationColorPurple;		
		return pinView;
	} else if ([annotation isKindOfClass:[MyAnnotation5 class]]) {
		MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
		UIImage *uavImage = [UIImage imageNamed:@"blue_wp.png"];
		uavView.image=uavImage;
		uavView.opaque = YES; 
		uavView.centerOffset = CGPointMake(7,-15);
		[uavImage release];	
		return uavView;	
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
	} else if ([annotation isKindOfClass:[MyAnnotation4 class]]) {
		MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"UAV3"] autorelease];
		UIImage *uavImage = [UIImage imageNamed:@"uav3.png"];
		uavView.canShowCallout = YES;			
		uavView.image=uavImage;
		uavView.opaque = YES;  
		[uavImage release];	
		return uavView;	
	} else if ([annotation isKindOfClass:[MyAnnotation6 class]]) {
		MKAnnotationView *uavView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"UAV4"] autorelease];
		UIImage *uavImage = [UIImage imageNamed:@"uav4.png"];
		uavView.canShowCallout = YES;			
		uavView.image=uavImage;
		uavView.opaque = YES;  
		[uavImage release];	
		return uavView;	
	}
	return nil;
}

// for overlay
- (MKOverlayView *) mapView:(MKMapView *)theMapView viewForOverlay:(id <MKOverlay>) overlay
{
	if ([overlay isKindOfClass:[MKPolygon class]]) {
		MKPolygonView *aView = [[[MKPolygonView alloc] initWithPolygon:(MKPolygon*)overlay]  autorelease];
		aView.fillColor = [[UIColor yellowColor] colorWithAlphaComponent:0.2];
		aView.lineWidth = 1;
		return aView;	
	} else if ([overlay isKindOfClass:[MKPolyline class]]) {
		MKPolylineView *aView = [[[MKPolylineView alloc] initWithPolyline:(MKPolyline*)overlay]  autorelease];
		aView.fillColor = [UIColor blackColor];
		aView.strokeColor = [UIColor blackColor];
		aView.lineWidth = 1;
		return aView;	
	}
	return nil;
}

// determine which area to display overlay 
- (void)addOverlayArea:(int *)data
{
	if (algID != 1) {
	NSTimeInterval currTime = [[NSDate date] timeIntervalSince1970]*1000;
	if (currTime > overLastTime+(interval/2)) {
		overLastTime = [[NSDate date] timeIntervalSince1970]*1000;
		
		int rows = data[0];
		int cols = data[1];
		int bArray[rows][cols];
		int i, j, k=2, l, m, ip=0, numip=0;
	
		for (i=0; i<cols; i++) {
			for (j=0; j<rows; j++) {
				bArray[j][i] = data[k];
				k+=1; 
			}
		}
		
		CLLocationCoordinate2D points[rows], inpoints[rows], inpoints2[rows], inpoints3[rows];
		j=0, k=0, l=0, m=0;

		[self.mapView removeOverlays:allpoly];
		allpoly=[[NSMutableArray alloc] init];
		for (int i=0; i<rows*cols; i++) {	
			if (bArray[i][0] == -2 && bArray[i][1] == -2 ) {
				ip=1; numip+=1;
			} else if (bArray[i][0] >= 0 && bArray[i][1] >= 0 && ip==0) {
				MKMapPoint p = MKMapPointMake(bArray[i][0]+MAPPOINTX,bArray[i][1]+MAPPOINTY);		
				CLLocationCoordinate2D coord = MKCoordinateForMapPoint(p);
				points[j] = coord;
				j+=1;
			} else if (bArray[i][0] >= 0 && bArray[i][1] >= 0 && ip==1) {
				if (numip==1) {
					MKMapPoint p = MKMapPointMake(bArray[i][0]+MAPPOINTX,bArray[i][1]+MAPPOINTY);		
					CLLocationCoordinate2D coord = MKCoordinateForMapPoint(p);
					inpoints[k] = coord;
					k+=1;
				} else if (numip==2) {
					MKMapPoint p = MKMapPointMake(bArray[i][0]+MAPPOINTX,bArray[i][1]+MAPPOINTY);		
					CLLocationCoordinate2D coord = MKCoordinateForMapPoint(p);
					inpoints2[l] = coord;
					l+=1;
				} else if (numip==3) {
						MKMapPoint p = MKMapPointMake(bArray[i][0]+MAPPOINTX,bArray[i][1]+MAPPOINTY);		
						CLLocationCoordinate2D coord = MKCoordinateForMapPoint(p);
						inpoints3[m] = coord;
						m+=1;
					}
			} else if (bArray[i][0] == -1 && bArray[i][1] == -1 && j <= 2){
				m=0, l=0, k=0, j=0, ip=0, numip=0;
			} else if (bArray[i][0] == -1 && bArray[i][1] == -1 && j > 2){
				if (ip==0) 
					poly = [MKPolygon polygonWithCoordinates:points count:j];
				else {
					if (numip==1) {
						poly2 = [MKPolygon polygonWithCoordinates:inpoints count:k];
						poly = [MKPolygon polygonWithCoordinates:points count:j interiorPolygons:[NSArray arrayWithObject:poly2]];
					} else if (numip==2) {
						poly2 = [MKPolygon polygonWithCoordinates:inpoints count:k];
						poly3 = [MKPolygon polygonWithCoordinates:inpoints2 count:l];
						poly = [MKPolygon polygonWithCoordinates:points count:j interiorPolygons:[NSArray arrayWithObjects:poly2,poly3,nil]];
					} else if (numip==3) {
						poly2 = [MKPolygon polygonWithCoordinates:inpoints count:k];
						poly3 = [MKPolygon polygonWithCoordinates:inpoints2 count:l];
						poly4 = [MKPolygon polygonWithCoordinates:inpoints3 count:m];
						poly = [MKPolygon polygonWithCoordinates:points count:j interiorPolygons:[NSArray arrayWithObjects:poly2,poly3,poly4,nil]];
					}
				}
				[allpoly addObject:poly];
				m=0, l=0, k=0, j=0, ip=0, numip=0;
			}
		}
		[mapView addOverlays:allpoly]; 
	}
	}
}

// add route line for each uav
- (void) addRouteLine:(int *)data
 {
	 if (algID != 1) {
		 CLLocationCoordinate2D points[2];
 
		 for (int i=2; i<10; i+=2) {	
			 MKMapPoint p = MKMapPointMake(data[i]+MAPPOINTX,data[i+1]+MAPPOINTY);
			 MKMapPoint r = MKMapPointMake(data[i+8]+MAPPOINTX,data[i+8+1]+MAPPOINTY);	
 
			 //NSLog(@"route: r=%f,%f", r.x-MAPPOINTX, r.y-MAPPOINTY);

			 CLLocationCoordinate2D coord1 = MKCoordinateForMapPoint(p);
			 CLLocationCoordinate2D coord2 = MKCoordinateForMapPoint(r);
			 points[0] = coord1, points[1] = coord2;
			 
			 if (i==2) {
				 [mapView removeOverlay:polyl];
				 polyl = [MKPolyline polylineWithCoordinates:points count:2];
				 [mapView addOverlay:polyl]; 
			 }
			 if (i==4) {
				 [mapView removeOverlay:polyl2];
				 polyl2 = [MKPolyline polylineWithCoordinates:points count:2];
				 [mapView addOverlay:polyl2];
			 }
			 if (i==6) {
				 [mapView removeOverlay:polyl3];
				 polyl3 = [MKPolyline polylineWithCoordinates:points count:2];
				 [mapView addOverlay:polyl3];
			 }
			 if (i==8) {
				 [mapView removeOverlay:polyl4];
				 polyl4 = [MKPolyline polylineWithCoordinates:points count:2];
				 [mapView addOverlay:polyl4];
			 }
		 }	
	 }
 }

// moves uav according to data received from Webots
- (void)moveUAVs:(int *)data
{
	if (d==0) { //send dummy value to server
		NSString *waypointStr = [NSString stringWithFormat:@"1,1,0,200,1"];		
		[socket2 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending dummy waypoint2: %@ %i",waypointStr, algID);
		d=1;
	}
	
	NSTimeInterval currTime = [[NSDate date] timeIntervalSince1970]*1000;
	if (currTime > mvLastTime+interval) {
		//NSLog(@"moveUAVs: p1=%f,%f p2=%f,%f %f", p1.x-MAPPOINTX, p1.y-MAPPOINTY, p2.x-MAPPOINTX, p2.y-MAPPOINTY, currTime);
		mvLastTime = [[NSDate date] timeIntervalSince1970]*1000;
	
		MKMapPoint p1 = MKMapPointMake(data[2]+MAPPOINTX,data[3]+MAPPOINTY);////
		MKMapPoint p2 = MKMapPointMake(data[4]+MAPPOINTX,data[5]+MAPPOINTY);
	
		CLLocationCoordinate2D uavCoord1 = MKCoordinateForMapPoint(p1);
		CLLocationCoordinate2D uavCoord2 = MKCoordinateForMapPoint(p2);
		CLLocationCoordinate2D uavCoord3, uavCoord4;
	
		if (numUAV >= 3) { 
			MKMapPoint p3 = MKMapPointMake(data[6]+MAPPOINTX,data[7]+MAPPOINTY);
			uavCoord3 = MKCoordinateForMapPoint(p3);
		}
		if (numUAV == 4) { 
			MKMapPoint p4 = MKMapPointMake(data[8]+MAPPOINTX,data[9]+MAPPOINTY);
			uavCoord4 = MKCoordinateForMapPoint(p4);
		}

		[self.mapView removeAnnotation:myAnn1]; 
		[self.mapView removeAnnotation:myAnn2]; 
		[self.mapView removeAnnotation:myAnn4]; 
		[self.mapView removeAnnotation:myAnn6];
		
		myAnn1 = [[MyAnnotation1 alloc] init];
		myAnn1.latitude = [NSNumber numberWithDouble:uavCoord1.latitude];
		myAnn1.longitude = [NSNumber numberWithDouble:uavCoord1.longitude];	
		[self.mapView addAnnotation:myAnn1];
		[myAnn1 release];	
	
		myAnn2 = [[MyAnnotation2 alloc] init];
		myAnn2.latitude = [NSNumber numberWithDouble:uavCoord2.latitude];
		myAnn2.longitude = [NSNumber numberWithDouble:uavCoord2.longitude];	
		[self.mapView addAnnotation:myAnn2];
		[myAnn2 release];
		
		if (numUAV >= 3) { 
			myAnn4 = [[MyAnnotation4 alloc] init];
			myAnn4.latitude = [NSNumber numberWithDouble:uavCoord3.latitude];
			myAnn4.longitude = [NSNumber numberWithDouble:uavCoord3.longitude];	
			[self.mapView addAnnotation:myAnn4];
			[myAnn4 release];		
		}
		if (numUAV == 4) { 
			myAnn6 = [[MyAnnotation6 alloc] init];
			myAnn6.latitude = [NSNumber numberWithDouble:uavCoord4.latitude];
			myAnn6.longitude = [NSNumber numberWithDouble:uavCoord4.longitude];	
			[self.mapView addAnnotation:myAnn6];
			[myAnn6 release];		
		}
	}
}

// determine if uav reached waypoint
- (void)wpReached:(int *)data
{
	int threshold = 5;
	
	if (wpReach1 < wpReach) { //uav1
		int a = [[pointWaypoint objectAtIndex:wpReach1] intValue];
		if (a == 1) {
			int b = [[pointWaypoint objectAtIndex:wpReach1+1] intValue];
			int c = [[pointWaypoint objectAtIndex:wpReach1+2] intValue];
			float dist = [self distEq:data[2]:data[3]:b:c];	
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav1 waypoint %i %i reached %f", b, c, wpTime);
				[self.pointWaypoint replaceObjectAtIndex:wpReach1+4 withObject:[NSNumber numberWithDouble:wpTime]];
				wpReach1 +=5;			
			}
		} else if ((a == 2 || a == 3 || a == 4) && wpReach1 < wpReach) {
			wpReach1 += 5;
		}
	} 
	if (wpReach2 < wpReach) { //uav2
		int a = [[pointWaypoint objectAtIndex:wpReach2] intValue];
		if (a == 2) {
			int b = [[pointWaypoint objectAtIndex:wpReach2+1] intValue];
			int c = [[pointWaypoint objectAtIndex:wpReach2+2] intValue];
			float dist = [self distEq:data[4]:data[5]:b:c];		
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav2 waypoint %i %i reached %f", b, c, wpTime);
				[self.pointWaypoint replaceObjectAtIndex:wpReach2+4 withObject:[NSNumber numberWithDouble:wpTime]];
				wpReach2 +=5;
			}
		} else if ((a == 1 || a == 3 || a == 4) && wpReach2 < wpReach) {
			wpReach2 += 5;
		}
	}
	if ((numUAV >= 3) && wpReach3 < wpReach) { //uav3
		int a = [[pointWaypoint objectAtIndex:wpReach3] intValue];
		if (a == 3) {
			int b = [[pointWaypoint objectAtIndex:wpReach3+1] intValue];
			int c = [[pointWaypoint objectAtIndex:wpReach3+2] intValue];
			float dist = [self distEq:data[6]:data[7]:b:c];		
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav3 waypoint %i %i reached %f", b, c, wpTime);
				[self.pointWaypoint replaceObjectAtIndex:wpReach3+4 withObject:[NSNumber numberWithDouble:wpTime]];
				wpReach3 += 5;
			}
		} else if ((a == 1 || a == 2 || a == 4) && wpReach3 < wpReach) {
			wpReach3 += 5;
		}
	}
	if ((numUAV == 4) && wpReach4 < wpReach) { //uav4
		int a = [[pointWaypoint objectAtIndex:wpReach4] intValue];
		if (a == 4) {
			int b = [[pointWaypoint objectAtIndex:wpReach4+1] intValue];
			int c = [[pointWaypoint objectAtIndex:wpReach4+2] intValue];
			float dist = [self distEq:data[8]:data[9]:b:c];			
			if (dist < threshold) {
				NSTimeInterval wpTime = [[NSDate date] timeIntervalSince1970]*1000;
				NSLog(@"uav4 waypoint %i %i reached %f", b, c, wpTime);
				[self.pointWaypoint replaceObjectAtIndex:wpReach4+4 withObject:[NSNumber numberWithDouble:wpTime]];
				wpReach4 +=5;
			}
		} else if ((a == 1 || a == 2 || a == 3) && wpReach4 < wpReach) {
			wpReach4 += 5;
		}
	}
}

// checks if target is in view
- (void)targetInView:(int *)data
{
	int threshold = 108; 
	float dist1, dist2, dist3, dist4;
	NSTimeInterval tgtTime = [[NSDate date] timeIntervalSince1970]*1000;
				
	if (data[2] != 0 && data[3] != 0) { // uav1
		dist1 = [self distEq:data[2]:data[3]:data[0]:data[1]];	
		if (initDist == 0) {
			NSLog(@"init dist between uav (%i,%i) and target (%i,%i): %f", data[2], data[3], data[0], data[1], dist1);
			initDist = 1;	
		}	
		if (dist1 < threshold) { 
			if (tgtTime > tgtLastTime1+interval) {
				NSLog(@"Target in UAV 1 view dist: %f %f", dist1, tgtTime);
				tgtLastTime1 = [[NSDate date] timeIntervalSince1970]*1000;			
			}
		}
	}								  						
	if (data[4] != 0 && data[5] != 0) {	// uav2
		dist2 = [self distEq:data[4]:data[5]:data[0]:data[1]];	
		if (dist2 < threshold) { 
			if (tgtTime > tgtLastTime2+interval) {
				NSLog(@"Target in UAV 2 view dist: %f %f", dist2, tgtTime);
				tgtLastTime2 = [[NSDate date] timeIntervalSince1970]*1000;				
			}
		}
	}				
	if ((numUAV >= 3) && data[6] != 0 && data[7] != 0) { // uav3	
		dist3 = [self distEq:data[6]:data[7]:data[0]:data[1]];	
		if (dist3 < threshold) { 
			if (tgtTime > tgtLastTime3+interval) {
				NSLog(@"Target in UAV 3 view dist: %f %f", dist3, tgtTime);
				tgtLastTime3 = [[NSDate date] timeIntervalSince1970]*1000;				
			}
		}
	}
	if ((numUAV == 4) && data[8] != 0 && data[9] != 0) { // uav4	
		dist4 = [self distEq:data[8]:data[9]:data[0]:data[1]];	
		if (dist4 < threshold) { 
			if (tgtTime > tgtLastTime4+interval) {
				NSLog(@"Target in UAV 4 view dist: %f %f", dist4, tgtTime);
				tgtLastTime4 = [[NSDate date] timeIntervalSince1970]*1000;				
			}
		}
	}
	
	// keeps up with total dist traveled
	if ((tgtTime > tgtLastTime+interval) && ((data[2] > 0) || (data[4] > 0) || (data[6] > 0) || (data[8] > 0))) {
		if (st == 0) {
			startMove = [[NSDate date] timeIntervalSince1970]*1000;
			NSLog(@"Start moving %f", startMove);
			st=1;
		}
		NSLog(@"target=%i,%i p1=%i,%i p2=%i,%i p3=%i,%i p4=%i,%i dist1=%f dist2=%f dist3=%f dist4=%f %f", 
			  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], dist1, dist2, dist3, dist4, tgtTime);		
		tgtLastTime = [[NSDate date] timeIntervalSince1970]*1000;	
	
		float tdist1 = [self distEq:data[2]:data[3]:lastX1:lastY1];
		totalDist1 += tdist1;
		lastX1 = data[2], lastY1 = data[3];
		
		float tdist2 = [self distEq:data[4]:data[5]:lastX2:lastY2];
		totalDist2 += tdist2;
		lastX2 = data[4], lastY2 = data[5];	
		
		if (numUAV >= 3) {
			float tdist3 = [self distEq:data[6]:data[7]:lastX3:lastY3];
			totalDist3 += tdist3;
			lastX3 = data[6], lastY3 = data[7];	
		}
		
		if (numUAV == 4) {
			float tdist4 = [self distEq:data[8]:data[9]:lastX4:lastY4];
			totalDist4 += tdist4;
			lastX4 = data[8], lastY4 = data[9];	
		}
	}
}

- (float) distEq:(int)x1:(int)y1:(int)x2:(int)y2
{
	int x = pow(x1 - x2, 2.0);
	int y = pow(y1 - y2, 2.0);
	float dist = sqrt((float)(x+y));
	return dist;
}

////////////////////////////////////////////////////

// get the coordinates passed from MapViewController
- (void) setPoint:(NSMutableArray *)passedCoord:(int)passedAlg:(NSString *)passedStr:(int)passedUAV
{
	initCoord = passedCoord;
	algID = passedAlg;
	numUAV = passedUAV;
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
	if ([initCoord count]==0) { //use default ROI
		coord.latitude=33.2131;
		coord.longitude=-87.5426;	
		region.center=coord;
		MKCoordinateSpan span = {0.001, 0.001};
		region.span = span;
	} else { //use coordinates chosen by user to display ROI
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
	initDist = 0, wpReach = 0, wpReach1 = 0, wpReach2 = 0, wpReach3;
	waypointUAV = 0;
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
	
	rawData1 = malloc(width*height*4);
	rawData2 = malloc(width*height*4);
	rawData3 = malloc(width*height*4);
	rawData4 = malloc(width*height*4);
	
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
	
	free(rawData1);
	free(rawData2);
	free(rawData3);
	free(rawData4);
	
	[super dealloc];
}
	
@end
