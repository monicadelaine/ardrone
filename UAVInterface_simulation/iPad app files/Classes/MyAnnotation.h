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
@property (nonatomic, assign, readonly) CLLocationCoordinate2D _coordinate;

@end

@interface MyAnnotation0 : NSObject <MKAnnotation>
{
    CLLocationCoordinate2D _coordinate;
    NSNumber *latitude;
    NSNumber *longitude;
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@property (nonatomic, retain) NSNumber *latitude;
@property (nonatomic, retain) NSNumber *longitude;
@property (nonatomic, assign, readonly) CLLocationCoordinate2D _coordinate;

@end

@interface MyAnnotation1 : NSObject <MKAnnotation>
{
    CLLocationCoordinate2D _coordinate;
    NSNumber *latitude;
    NSNumber *longitude;
	UIImage *image;
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@property (nonatomic, retain) NSNumber *latitude;
@property (nonatomic, retain) NSNumber *longitude;
@property (nonatomic, retain) UIImage *image;
@property (nonatomic, assign, readonly) CLLocationCoordinate2D _coordinate;

@end


@interface MyAnnotation2 : NSObject <MKAnnotation>
{
    CLLocationCoordinate2D _coordinate;
    NSNumber *latitude;
    NSNumber *longitude;
	UIImage *image;
}

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@property (nonatomic, retain) NSNumber *latitude;
@property (nonatomic, retain) NSNumber *longitude;
@property (nonatomic, retain) UIImage *image;
@property (nonatomic, assign, readonly) CLLocationCoordinate2D _coordinate;

@end