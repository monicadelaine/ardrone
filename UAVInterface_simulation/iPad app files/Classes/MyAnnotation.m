//
//  MyAnnotation.m


#import "MyAnnotation.h"


@implementation MyAnnotation

@synthesize latitude;
@synthesize longitude;
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
    return @"Testing";
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