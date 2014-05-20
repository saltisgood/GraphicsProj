#ifndef DISPLAY_H_
#define DISPLAY_H_

namespace disp
{
	class Display
	{
	public:
		Display(int width, int height) : mWindowWidth(width), mWindowHeight(height) {}
		virtual ~Display() {}

		int getWidth() const { return mWindowWidth; }
		int getHeight() const { return mWindowHeight; }

	private:
		const int mWindowWidth;
		const int mWindowHeight;
	};
}

#endif