//
//  MyAnnotation.m


#import "MyAnnotation.h"


@implementation MyAnnotation

@synthesize latitude;
@synthesize longitude;
@synthesize image;
@synthesize _coordinate;

- (CLLocationCoordinate2D)coordinate;
{
    CLLocationCoordinate2D theCoordinate;

	theCoordinate.latitude = self.latitude.doubleValue;
    theCoordinate.longitude = self.longitude.doubleValue;
    return theCoordinate;
}

- (NSString *)title
{
    return @"UAV";
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