//
//  MyAnnotation.h


#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface MyAnnotation : NSObject <MKAnnotation>
{
    CLLocationCoordinate2D _coordinate;
    NSNumber *latitude;
    NSNumber *longitude;
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@property (nonatomic, retain) NSNumber *latitude;
@property (nonatomic, retain) NSNumber *longitude;

// the coordinate is a derived property based on the properties latitude and longitude.
@property (nonatomic, assign, readonly) CLLocationCoordinate2D _coordinate;

@end