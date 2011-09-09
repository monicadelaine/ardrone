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

- (NSString *)title
{
    return @"UAV1 WP";
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
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

- (NSString *)title
{
    return @"UAV2 WP";
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

@end


@implementation MyAnnotation1

@synthesize latitude, longitude, image, _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;
	
	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title
{
    return @"UAV1";
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
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

- (NSString *)title
{
    return @"UAV2";
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate
{
	_coordinate=coordinate;
	NSLog(@"%f,%f",coordinate.latitude,coordinate.longitude);
	return self;
}

- (void)dealloc
{
	[image	release];
    [super dealloc];
}

@end