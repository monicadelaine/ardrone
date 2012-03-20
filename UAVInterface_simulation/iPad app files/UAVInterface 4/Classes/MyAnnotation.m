//
//  MyAnnotation.m

#import "MyAnnotation.h"

@implementation MyAnnotation

@synthesize latitude, longitude, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV1 WP"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc { 
	[latitude release];
	[longitude release];
	[super dealloc]; 
}

@end


@implementation MyAnnotation0

@synthesize latitude, longitude, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;	
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV2 WP"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc { 
	[latitude release];
	[longitude release];
	[super dealloc]; 
}

@end


@implementation MyAnnotation3

@synthesize latitude, longitude, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;	
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV3 WP"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc { 
	[latitude release];
	[longitude release];
	[super dealloc]; 
}

@end


@implementation MyAnnotation5

@synthesize latitude, longitude, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;	
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV4 WP"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc { 
	[latitude release];
	[longitude release];
	[super dealloc]; 
}

@end

//////////////////////////////////////////////////////////////////////////////

@implementation MyAnnotation1

@synthesize latitude, longitude, image, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;	
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV1"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
	[latitude release];
	[longitude release];
	[image	release];
    [super dealloc];
}

@end


@implementation MyAnnotation2

@synthesize latitude, longitude, image, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV2"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
	[latitude release];
	[longitude release];
	[image	release];
    [super dealloc];
}

@end


@implementation MyAnnotation4

@synthesize latitude, longitude, image, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV3"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
	[latitude release];
	[longitude release];
	[image	release];
    [super dealloc];
}

@end


@implementation MyAnnotation6

@synthesize latitude, longitude, image, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title { return @"UAV4"; }

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
	[latitude release];
	[longitude release];
	[image	release];
    [super dealloc];
}

@end