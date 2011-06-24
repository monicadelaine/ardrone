//
//  RoiViewController.m

#import <QuartzCore/QuartzCore.h>

#import "RoiViewController.h"
#import "MapViewController.h"
#import "MyAnnotation.h"

#define ROI_X_MAX 318
#define ROI_Y_MAX 285
#define IMAGE_ARRAY 12290

enum
{
    kAnnotationIndex = 0,
};	


@implementation RoiViewController

@synthesize mapView, initCoord, initPoint, waypoint, pointWaypoint;
@synthesize socket, socket2;

double latSpan, lonSpan, centerCoordLat, centerCoordLon;
double xScale, yScale;

CGPoint point, newPoint1, newPoint2;
UIImageView *myImage1, *myImage2;
MyAnnotation *myAnn;
NSString *pointStr;

int enlargeFlag = 0, enlargeButton = 0, waypointUAV = 0, waypointButton = 0;
int width = 64, height = 64;

NSString * host = @"130.160.68.24";
UInt16 iport = 1501;
UInt16 oport = 1506;

//connect to server
-(void)connectToServer
{
	NSLog(@"Connecting To Server");
	NSError *error = nil;
	
	if ([socket connectToHost:host onPort:iport error:&error]) { // connect to socket
		NSLog(@"Connected on port %i", iport);
	} else {
		NSLog(@"Error connecting to server at %i: %@", iport, error);
	}
		
	[self sendDataToServer:pointStr]; //send data
	[self receiveDataFromServer]; //receive data
}

//send data to server
-(void)sendDataToServer:(NSString *)data
{
	[socket sendData:[data dataUsingEncoding:NSASCIIStringEncoding] withTimeout:-1 tag:0];
	NSLog (@"Sending: %@",data);
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
	unsigned char *byteData = (unsigned char *)[data bytes];
	NSMutableArray *posData = [[NSMutableArray alloc] init];
	
	if (byteData[0] == 'A') {
		[self displayImage1:byteData]; 
		[posData insertObject:[NSNumber numberWithDouble:byteData[IMAGE_ARRAY+3]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData[IMAGE_ARRAY+2]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData[IMAGE_ARRAY+1]]atIndex:kAnnotationIndex];
		[posData insertObject:[NSNumber numberWithDouble:byteData[IMAGE_ARRAY]]atIndex:kAnnotationIndex];		
		[self moveUAVs:posData]; 		
	} else if (byteData[0] == 'B') {
		[self displayImage2:byteData]; 
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
	
	//display image1
	CGRect myImageRect1;
	if (enlargeFlag == 0) {
		myImageRect1 = CGRectMake(0.0f, 328.0f, 162.0f, 88.0f); 
	} else if (enlargeFlag == 1) {
		myImageRect1 = CGRectMake(0.0f, 0.0f, 320.0f, 418.0f); 
	}
	/*UIImageView **/myImage1 = [[UIImageView alloc] initWithFrame:myImageRect1]; 
	[myImage1 setImage:newImage1]; 
	myImage1.opaque = YES;  
	if (enlargeFlag == 0) {
		
		myImage1.frame = CGRectMake(0.0, 328.0, 162.0, 88.0);
		myImage1.layer.borderColor = [UIColor redColor].CGColor;
		myImage1.layer.borderWidth = 1.0;
	} 	
	[self.view addSubview:myImage1];
	//myImage1.image = nil; //try to clear the image  
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
	
	CGRect myImageRect2;
	if (enlargeFlag == 0) {
		myImageRect2 = CGRectMake(162.0f, 328.0f, 162.0f, 88.0f); 
	} else if (enlargeFlag == 2) {
		myImageRect2 = CGRectMake(0.0f, 0.0f, 320.0f, 418.0f); 
		
	}	
	/*UIImageView **/myImage2 = [[UIImageView alloc] initWithFrame:myImageRect2]; 
	myImage2.image = nil; //try to clear the image 
	[myImage2 setImage:newImage2]; 
	myImage2.opaque = YES;  
	if (enlargeFlag == 0) {
		myImage2.frame = CGRectMake(162.0, 328.0, 162.0, 88.0);
		myImage2.layer.borderColor = [UIColor blueColor].CGColor;
		myImage2.layer.borderWidth = 1.0;
	} 	
	[self.view addSubview:myImage2]; 
	//myImage2.image = nil; //try to clear the image
	[myImage2 release];		
}

- (IBAction)options:(id)sender 
{
	switch (((UISegmentedControl *)sender).selectedSegmentIndex)
    {
        case 0:
        {
			//action for ROI
			NSLog(@"ROI button");
			myImage1.image = nil; //try to clear the image  
			myImage2.image = nil; 
			self.mapView.hidden = NO; //try to show map again
			[self.mapView removeAnnotations:mapView.annotations];
			enlargeFlag = 0;
			break;
        } 
		case 1:
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
			break;
        } 
        case 2:
        {	
			//action for Enlarge
			NSLog(@"Enlarge button");
			enlargeButton=1;
			self.mapView.hidden = YES;	//try to hide map	
			alertButton = [[UIAlertView alloc] initWithTitle:@"" message:@"Enlarge video for" delegate:self cancelButtonTitle:@"UAV1" otherButtonTitles:@"UAV2", nil];
			[alertButton show];
			[alertButton release];
            break;
        } 
    }			
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
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

- (MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation
{ 
	if ([annotation isKindOfClass:[MyAnnotation class]]) {
		MKPinAnnotationView *pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"WAYPOINT"] autorelease];
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
		UIImage *uavImage = [UIImage imageNamed:@"uav2.png"];
		uavView.canShowCallout = YES;			
		uavView.image=uavImage;
		uavView.opaque = YES;  
		[uavImage release];	
		return uavView;	
	}
	return nil;
}

//TODO: clear annotation for old uav position
- (void)moveUAVs:(NSMutableArray *)data
{
	for (int i=0; i<4; i++) {
		NSLog(@"uav pos %i = %@", i, [data objectAtIndex:i]);
	}	
	CGPoint point1, point2;
	point1 = CGPointMake([[data objectAtIndex:0] doubleValue],[[data objectAtIndex:1]doubleValue]);
	point2 = CGPointMake([[data objectAtIndex:2] doubleValue],[[data objectAtIndex:3]doubleValue]);	

	[self mapToRoi:point1.x:point1.y:point2.x:point2.y];
	
	CLLocationCoordinate2D uavCoord1 = [self.mapView convertPoint:newPoint1 toCoordinateFromView:self.mapView];
	CLLocationCoordinate2D uavCoord2 = [self.mapView convertPoint:newPoint2 toCoordinateFromView:self.mapView];
	
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
}

- (void)handleLongPressGesture:(UIGestureRecognizer*)sender 
{
	myAnn = [[MyAnnotation alloc] init];	
	[waypoint removeAllObjects];
	[pointWaypoint removeAllObjects];	
	if (sender.state==UIGestureRecognizerStateBegan) {
		point = [sender locationInView:self.mapView];
		CLLocationCoordinate2D locCoord = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.longitude]atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:locCoord.latitude] atIndex:kAnnotationIndex];
		[self.waypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];
		
		myAnn.latitude = [NSNumber numberWithDouble:locCoord.latitude];
		myAnn.longitude = [NSNumber numberWithDouble:locCoord.longitude];	
		[self.mapView addAnnotation:myAnn];
		[myAnn release];	
		
		[self mapToArea:point.x:point.y];
		
		[self.pointWaypoint insertObject:[NSNumber numberWithDouble:point.y] atIndex:kAnnotationIndex];	
		[self.pointWaypoint insertObject: [NSNumber numberWithDouble:point.x] atIndex:kAnnotationIndex];
		[self.pointWaypoint insertObject:[NSNumber numberWithDouble:waypointUAV] atIndex:kAnnotationIndex];		
		
		int count=[waypoint count];
		for (int i=0; i<count; i++) {
			NSLog(@"waypoint coord %i = %@", i, [waypoint objectAtIndex:i]);
		}
		int pcnt=[pointWaypoint count];
		for (int i=0; i<pcnt; i++) {
			NSLog(@"waypoint point %i = %@", i, [pointWaypoint objectAtIndex:i]);
		}

		//send waypoint to server
		NSString *waypointStr = [NSString stringWithFormat:@"%i,%f,%f",waypointUAV,point.x,point.y];
		[socket2 sendData:[waypointStr dataUsingEncoding:NSASCIIStringEncoding] toHost:(NSString *)host port:(UInt16)oport withTimeout:-1 tag:0];
		NSLog (@"Sending: %@",waypointStr);
	}	
} 

- (void) mapToRoi:(double)x1:(double)y1:(double)x2:(double)y2
{
	if ([initPoint count]!=0) {
		//newx=xscale*(goalx-p1x), newy=yscale*(goaly-p1y)
		double newX1 = xScale * (x1-[[initPoint objectAtIndex:6] doubleValue]);
		double newY1 = yScale * (y1-[[initPoint objectAtIndex:7] doubleValue]);
		//NSLog(@"converted area %f %f to roi %f %f for uav 1",x1,y1,newX1,newY1);
		
		double newX2 = xScale * (x2-[[initPoint objectAtIndex:6] doubleValue]);
		double newY2 = yScale * (y2-[[initPoint objectAtIndex:7] doubleValue]);
		//NSLog(@"converted area %f %f to roi %f %f for uav 2",x2,y2,newX2,newY2);	
		
		newPoint1 = CGPointMake(newX1,newY1);
		newPoint2 = CGPointMake(newX2,newY2);	
	}
}

- (void) mapToArea:(double)x1:(double)y1
{
	if ([initPoint count]!=0) {
		//newX=(goalx/xscale)+p1x, newY=(goaly/yscale)+p1y
		double newX1 = (x1 / xScale) + [[initPoint objectAtIndex:6] doubleValue];
		double newY1 = (y1 / yScale) + [[initPoint objectAtIndex:7] doubleValue];
		NSLog(@"converted roi %f %f to area %f %f",x1,y1,newX1,newY1);
		
		point.x = newX1;
		point.y = newY1;
	} 
}

- (void) setPoint:(NSMutableArray *)passedArray:(NSString *)passedStr
{
	initPoint = passedArray;
	
	int cnt=[initPoint count];
	if (cnt!=0) {
		for (int i=0; i<cnt; i++) {
			NSLog(@"init point %i = %@", i, [initPoint objectAtIndex:i]);
		}	
		
		//xScale=ROI_X_MAX/(p2x-p1x), yScale=ROI_Y_MAX/(p3y-p2y)
		xScale = (double)ROI_X_MAX / ([[initPoint objectAtIndex:4] doubleValue] - [[initPoint objectAtIndex:6] doubleValue]);
		yScale = (double)ROI_Y_MAX / ([[initPoint objectAtIndex:3] doubleValue] - [[initPoint objectAtIndex:5] doubleValue]);
		NSLog(@"scale: x=%f y=%f",xScale,yScale);	
	}
}

- (void) setCoord:(NSMutableArray *)passedArray
{
	initCoord = passedArray;
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
	//uses coordinates chosen by user to display ROI
	NSLog(@"ROI params: latSpan=%f lonSpan=%f centerLat=%f centerLon=%f", latSpan, lonSpan, centerCoordLat, centerCoordLon);
	
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
	
- (void)viewDidLoad
{
	//set ROI region
	[self regionSet]; 
	
	self.waypoint = [[NSMutableArray alloc] init];
	self.pointWaypoint = [[NSMutableArray alloc] init];
	
	//set up sockets and connect to server
	socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
	socket2 = [[AsyncUdpSocket alloc] initWithDelegate:self];
		
	[self connectToServer];
	
	mapView.delegate=self;
	
	[self setMap];
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
	[socket close];
	[socket2 close];
	[socket release];
	[socket2 release];	
	
	[super dealloc];
}

	
@end
