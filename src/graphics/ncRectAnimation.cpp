#include "ncRectAnimation.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets current frame
void ncRectAnimation::SetFrame(unsigned int uFrameNum)
{
	if (uFrameNum >= m_vRects.Size())
		m_uCurrentFrame = m_vRects.Size() - 1;
	else
		m_uCurrentFrame = uFrameNum;
}

/// Returns true if a frame rect has changed
void ncRectAnimation::UpdateFrame(unsigned long ulInterval)
{
	// No frame calculation if the animation is paused or has only one rect
	if (m_bPaused == true || m_vRects.Size() < 2)
		return;

	m_ulElapsedFrameTime += ulInterval;
	// A NEXT frame rectangle should be determined
	if (m_ulElapsedFrameTime >= m_uFrameTime)
	{
		m_ulElapsedFrameTime = 0;

		if (m_bGoingForward)
		{
			if (m_uCurrentFrame == m_vRects.Size() - 1)
			{
				if (m_bBackward)
				{
					m_bGoingForward = false;
					m_uCurrentFrame--;
				}
				else
				{
					if (m_bLooping == false)
						m_bPaused = true;
					else
						m_uCurrentFrame = 0;
				}
			}
			else
				m_uCurrentFrame++;
		}
		else
		{
			if (m_uCurrentFrame == 0)
			{
				if (m_bLooping == false)
					m_bPaused = true;
				else
				{
					m_bGoingForward = true;
					m_uCurrentFrame++;
				}
			}
			else
				m_uCurrentFrame--;
		}
	}
}

/// Pause on a specified frame
void ncRectAnimation::Pause(unsigned int uFrameNum)
{
	if (uFrameNum >= m_vRects.Size())
		m_uCurrentFrame = m_vRects.Size() - 1;
	else
		m_uCurrentFrame = uFrameNum;

	m_bPaused = true;
}