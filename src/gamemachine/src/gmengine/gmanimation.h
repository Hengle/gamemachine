﻿#ifndef __GMANIMATION_H__
#define __GMANIMATION_H__
#include <gmcommon.h>
BEGIN_NS

GM_PRIVATE_OBJECT(GMAnimationKeyframe)
{
	GMfloat time;
};

//! 动画的关键帧。
class GM_EXPORT GMAnimationKeyframe : public GMObject
{
	GM_DECLARE_PRIVATE(GMAnimationKeyframe)
	GM_DECLARE_PROPERTY(Time, time);

public:
	GMAnimationKeyframe(GMfloat timePoint);

	virtual void reset(IDestroyObject* object) = 0;
	virtual void beginFrame(IDestroyObject* object, GMfloat timeStart) = 0;
	virtual void endFrame(IDestroyObject* object) = 0;

	//! 关键帧处理方法，用于改变GMObject对象的状态。
	/*!
	  当当前关键帧为目标关键帧时，每当需要更新GMObject状态时，此方法被调用。
	  \param object 希望执行动画的对象。
	  \param time 当前动画执行的时间。从前一帧结束开始算起。
	*/
	virtual void update(IDestroyObject* object, GMfloat time) = 0;

public:
	bool operator <(const GMAnimationKeyframe& rhs) const
	{
		return getTime() < rhs.getTime();
	}
};

class GMAnimationKeyframeLess
{
public:
	bool operator ()(const GMAnimationKeyframe* lhs, const GMAnimationKeyframe* rhs)
	{
		return *lhs < *rhs;
	}
};

GM_PRIVATE_OBJECT(GMAnimation)
{
	bool playLoop = false;
	bool isPlaying = false;
	bool finished = false;
	GMDuration timeline = 0;
	Set<GMAnimationKeyframe*, GMAnimationKeyframeLess> keyframes;
	Set<GMAnimationKeyframe*, GMAnimationKeyframeLess>::const_iterator keyframesIter;
	Set<IDestroyObject*> targetObjects;
	GMAnimationKeyframe* lastKeyframe = nullptr;
};

class GM_EXPORT GMAnimation : public GMObject
{
	GM_DECLARE_PRIVATE(GMAnimation)
	GM_ALLOW_COPY_MOVE(GMAnimation)
	GM_DECLARE_PROPERTY(PlayLoop, playLoop)

public:
	template <typename... Objects>
	GMAnimation(Objects... targetGameObjects)
	{
		setTargetObjects(targetGameObjects...);
	}

#if GM_MSVC
	template <>
	GMAnimation(void) {};
#elif GM_GCC
	GMAnimation() = default;
#endif

	~GMAnimation();

public:
	template <typename... Object>
	void setTargetObjects(Object*... targetGameObjects)
	{
		D(d);
		d->targetObjects = { targetGameObjects... };
	}

	inline bool isPlaying() GM_NOEXCEPT
	{
		D(d);
		return d->isPlaying;
	}

	inline bool isFinished() GM_NOEXCEPT
	{
		D(d);
		return d->finished;
	}

public:
	void clearObjects();
	void clearFrames();
	void addKeyFrame(AUTORELEASE GMAnimationKeyframe* kf);
	void play();
	void pause();
	void reset();
	void update(GMDuration dt);

private:
	void updatePercentage();
};

// Animations
GM_PRIVATE_OBJECT(GMGameObjectKeyframe)
{
	Map<GMGameObject*, GMVec4, std::less<GMGameObject*>, AlignedAllocator<Pair<GMGameObject*, GMVec4>> > translationMap;
	Map<GMGameObject*, GMVec4, std::less<GMGameObject*>, AlignedAllocator<Pair<GMGameObject*, GMVec4>> > scalingMap;
	Map<GMGameObject*, GMQuat, std::less<GMGameObject*>, AlignedAllocator<Pair<GMGameObject*, GMQuat>> > rotationMap;
	GMVec4 translation;
	GMVec4 scaling;
	GMQuat rotation;
	GMfloat timeStart = 0;
};

class GM_EXPORT GMGameObjectKeyframe : public GMAnimationKeyframe
{
	GM_DECLARE_PRIVATE(GMGameObjectKeyframe)
	GM_DECLARE_PROPERTY(Translation, translation)
	GM_DECLARE_PROPERTY(Scaling, scaling)
	GM_DECLARE_PROPERTY(Rotation, rotation)

public:
	GMGameObjectKeyframe(
		const GMVec4& translation,
		const GMVec4& scaling,
		const GMQuat& rotation,
		GMfloat timePoint
	);

public:
	virtual void reset(IDestroyObject* object) override;
	virtual void beginFrame(IDestroyObject* object, GMfloat timeStart) override;
	virtual void endFrame(IDestroyObject* object) override;
	virtual void update(IDestroyObject* object, GMfloat time) override;
};

enum class GMCameraKeyframeComponent
{
	NoComponent,
	FocusAt,
	LookAtDirection,
};

GM_PRIVATE_OBJECT(GMCameraKeyframe)
{
	Map<GMCamera*, GMVec3, std::less<GMCamera*>, AlignedAllocator<Pair<GMGameObject*, GMVec3>> > positionMap;
	Map<GMCamera*, GMVec3, std::less<GMCamera*>, AlignedAllocator<Pair<GMGameObject*, GMVec3>> > lookAtDirectionMap;
	Map<GMCamera*, GMVec3, std::less<GMCamera*>, AlignedAllocator<Pair<GMGameObject*, GMVec3>> > focusMap;
	GMCameraKeyframeComponent component = GMCameraKeyframeComponent::NoComponent;
	GMVec3 position;
	GMVec3 lookAtDirection;
	GMVec3 focusAt;
	GMfloat timeStart = 0;
};

class GM_EXPORT GMCameraKeyframe : public GMAnimationKeyframe
{
	GM_DECLARE_PRIVATE_AND_BASE(GMCameraKeyframe, GMAnimationKeyframe)
	GM_DECLARE_PROPERTY(Position, position)
	GM_DECLARE_PROPERTY(LookAtDirection, lookAtDirection)
	GM_DECLARE_PROPERTY(FocusAt, focusAt)

public:
	GMCameraKeyframe(
		GMCameraKeyframeComponent component,
		const GMVec3& position,
		const GMVec3& lookAtDirectionOrFocusAt,
		GMfloat timePoint
	);

public:
	virtual void reset(IDestroyObject* object) override;
	virtual void beginFrame(IDestroyObject* object, GMfloat timeStart) override;
	virtual void endFrame(IDestroyObject* object) override;
	virtual void update(IDestroyObject* object, GMfloat time) override;
};

struct GMLightKeyframeComponent
{
	enum
	{
		NoComponent = 0x00,
		Ambient = 0x01,
		Diffuse = 0x02,
		Specular = 0x04,
	};
};

GM_PRIVATE_OBJECT(GMLightKeyframe)
{
	Map<ILight*, GMVec3, std::less<ILight*>, AlignedAllocator<Pair<GMGameObject*, GMVec3>> > ambientMap;
	Map<ILight*, GMVec3, std::less<ILight*>, AlignedAllocator<Pair<GMGameObject*, GMVec3>> > diffuseMap;
	Map<ILight*, GMfloat, std::less<ILight*>> specularMap;
	GMint32 component = GMLightKeyframeComponent::NoComponent;
	GMVec3 ambient;
	GMVec3 diffuse;
	GMfloat specular;
	GMfloat timeStart = 0;
	const IRenderContext* context = nullptr;
};

class GM_EXPORT GMLightKeyframe : public GMAnimationKeyframe
{
	GM_DECLARE_PRIVATE_AND_BASE(GMLightKeyframe, GMAnimationKeyframe)
	GM_DECLARE_PROPERTY(Ambient, ambient)
	GM_DECLARE_PROPERTY(Diffuse, diffuse)
	GM_DECLARE_PROPERTY(Specular, specular)

public:
	GMLightKeyframe(
		const IRenderContext* context,
		GMint32 component,
		const GMVec3& ambient,
		const GMVec3& diffuse,
		GMfloat specular,
		GMfloat timePoint
	);

public:
	virtual void reset(IDestroyObject* object) override;
	virtual void beginFrame(IDestroyObject* object, GMfloat timeStart) override;
	virtual void endFrame(IDestroyObject* object) override;
	virtual void update(IDestroyObject* object, GMfloat time) override;
};

END_NS
#endif