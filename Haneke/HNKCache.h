//
//  HNKCache.h
//  Haneke
//
//  Created by Hermes on 10/02/14.
//  Copyright (c) 2014 Hermes Pique. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol HNKCacheEntity;
@class HNKCacheFormat;

@interface HNKCache : NSObject

#pragma mark Initializing the cache
///---------------------------------------------
/// @name Initializing the cache
///---------------------------------------------

/**
 Initialize a cache with the given name.
 @param name Name of the cache. Used to as the name of the subdirectory for the disk cache.
*/
- (id)initWithName:(NSString*)name;

/**
 Returns the shared cache. For simple apps that don't require multiple caches.
 */
+ (HNKCache*)sharedCache;

/**
 Registers a format in the cache. The cache will automatically update the diskSize of the format as images are added.
 @param The format to be registered in the cache.
 @discussion A format can only be registered in one cache.
 */
- (void)registerFormat:(HNKCacheFormat*)format;

#pragma mark Getting images
///---------------------------------------------
/// @name Getting images
///---------------------------------------------

/**
 Synchronously retrieves an image from the cache, or creates it it doesn't exist. 
 @param entity Entity that represents the original image. If the image doesn't exist in the cache, the entity will be asked to provide the original data or image to create it.
 @param formatName Name of the format in which the image is desired. The format must have been previously registered with the cache. If the image doesn't exist in the cache, it will be created based on the format. If by creating the image the format disk capacity is surpassed, least recently used images of the format will be removed until it isn't.
 @discussion Disk operations such as saving the image are performed asynchronously, even if the image is returned synchronously.
 */
- (UIImage*)imageForEntity:(id<HNKCacheEntity>)entity formatName:(NSString *)formatName;

/**
 Retrieves an image from the cache, or creates it it doesn't exist. If the image exists in the memory cache, the completion block will be executed synchronously. If the image exists in the disk cache or has to be created, the completion block will be executed asynchronously.
 @param entity Entity that represents the original image. If the image doesn't exist in the cache, the entity will be asked to provide the original data or image to create it. Any calls to the entity will be done in the main queue.
 @param formatName Name of the format in which the image is desired. The format must have been previously registered with the cache. If the image doesn't exist in the cache, it will be created based on the format. If by creating the image the format disk capacity is surpassed, least recently used images of the format will be removed until it isn't.
 @param completionBlock The block to be called with the requested image. Always called from the main queue. Will be called synchronously if the image exists in the memory cache, or asynchronously if the image exists in the disk cache or had to be created.
 @return YES if image exists in the memory cache (and thus, the completion block was called synchronously), NO otherwise.
 */
- (BOOL)retrieveImageForEntity:(id<HNKCacheEntity>)entity formatName:(NSString *)formatName completionBlock:(void(^)(id<HNKCacheEntity> entity, NSString *formatName, UIImage *image))completionBlock;

#pragma mark Removing images
///---------------------------------------------
/// @name Removing images
///---------------------------------------------

/** Removes all images of the given format.
 @param formatName Name of the format whose images will be removed.
 */
- (void)clearFormatNamed:(NSString*)formatName;

/** Removes all images of the given entity.
 @param entity Entity whose images will be removed.
 */
- (void)removeImagesOfEntity:(id<HNKCacheEntity>)entity;

@end

/** Represents an object that is associated with an image. Used by the cache to assign identifiers to images and obtain the original data or image needed to create resized images. **/
@protocol HNKCacheEntity <NSObject>

/** 
 Return an identifier for the original image associated with the entity.
 @discussion If two different entities have the same image, they should return the same cache id for better performance.
 */
@property (nonatomic, readonly) NSString *cacheId;
/**
 Return the original image associated with the entity, or nil to use cacheOriginalData instead.
 */
@property (nonatomic, readonly) UIImage *cacheOriginalImage;
/**
 Return the original data associated with the entity, or nil to use cacheOriginalImage instead.
 */
@property (nonatomic, readonly) NSData *cacheOriginalData;


@end

typedef NS_ENUM(NSInteger, HNKScaleMode)
{
    HNKScaleModeFill = UIViewContentModeScaleToFill,
    HNKScaleModeAspectFit = UIViewContentModeScaleAspectFit,
    HNKScaleModeAspectFill = UIViewContentModeScaleAspectFill
};

@interface HNKCacheFormat : NSObject

@property (nonatomic, assign) BOOL allowUpscaling;
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, assign) CGSize size;
@property (nonatomic, assign) HNKScaleMode scaleMode;
/**
 The disk cache capacity for the format. If not set the default value is zero and the format will not have a disk cache.
 */
@property (nonatomic, assign) unsigned long long diskCapacity;
@property (nonatomic, readonly) unsigned long long diskSize;

- (id)initWithName:(NSString*)name;

/**
 Resized the given image based on the format. Used by the cache to create its images.
 @param image The original image.
 @return A resized image based on the format.
 */
- (UIImage*)resizedImageFromImage:(UIImage*)image;

@end
