import com.greensock.TimelineLite;
import com.greensock.easing.*;

class TargetLock.Reticle extends MovieClip
{
	public var ReticleTop: MovieClip;
	public var ReticleBottom: MovieClip;
	public var ReticleLeft: MovieClip;
	public var ReticleRight: MovieClip;
	public var ReticleSimpleDot: MovieClip;
	public var ReticleSimpleGlow: MovieClip;
	
	var initTimeline: TimelineLite;
	var rotationTimeline: TimelineLite;

	public function Reticle() 
	{
		// constructor code
		this.stop();
		
		initTimeline = new TimelineLite({paused:true, onComplete:function() { this.seek(0.5) }});
		rotationTimeline = new TimelineLite({paused:true, onComplete:function() { this.restart() }});
	}
	
	public function playTimeline() : Void
	{
		initTimeline.clear();
		initTimeline.set(ReticleTop, {_alpha:0}, 0)
		.set(ReticleBottom, {_alpha:0}, 0)
		.set(ReticleLeft, {_alpha:0}, 0)
		.set(ReticleRight, {_alpha:0}, 0)
		.set(ReticleSimpleDot, {_alpha:0}, 0)
		.set(ReticleSimpleGlow, {_alpha:0}, 0)
		.set(this, {_rotation:0, _xscale:300, _yscale:300}, 0)
		.to(this, 0.5, {_xscale:100, _yscale:100, ease:Sine.easeInOut}, 0)
		.to(ReticleTop, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(ReticleBottom, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(ReticleLeft, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(ReticleRight, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(ReticleSimpleDot, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(ReticleSimpleGlow, 0.5, {_alpha:100, ease:Sine.easeInOut}, 0)
		.to(this, 0.5, {_xscale:110, _yscale:110, ease:Sine.easeInOut}, 0.5)
		.to(this, 0.5, {_xscale:100, _yscale:100, ease:Sine.easeInOut}, 1)
		initTimeline.restart();

		rotationTimeline.clear();
		rotationTimeline.to(this, 8, {_rotation:"-=360", ease:Linear.easeNone}, 0);
		rotationTimeline.restart();
	}
	
	public function setReticleType(Type: Number) : Void
	{
		switch(Type)
		{
		case 0:
			ReticleTop._visible = true;
			ReticleBottom._visible = true;
			ReticleLeft._visible = true;
			ReticleRight._visible = true;
			ReticleSimpleDot._visible = false;
			ReticleSimpleGlow._visible = false;
			break;
		case 1:
			ReticleTop._visible = false;
			ReticleBottom._visible = false;
			ReticleLeft._visible = false;
			ReticleRight._visible = false;
			ReticleSimpleDot._visible = true;
			ReticleSimpleGlow._visible = false;
			break;
		case 2:
			ReticleTop._visible = false;
			ReticleBottom._visible = false;
			ReticleLeft._visible = false;
			ReticleRight._visible = false;
			ReticleSimpleDot._visible = false;
			ReticleSimpleGlow._visible = true;
			break;
		}
	}
	
	public function setReticleAlpha(Alpha: Number) : Void
	{
		_alpha = Alpha;
	}
}
