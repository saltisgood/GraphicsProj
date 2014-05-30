#ifndef DISPLAY_H_
#define DISPLAY_H_

namespace disp
{
	class Display
	{
	public:
		Display(int width, int height) : mWindowWidth(width), mWindowHeight(height) {}
		Display(const Display& d) : mWindowWidth(d.mWindowWidth), mWindowHeight(d.mWindowHeight) {}
		virtual ~Display() {}

		int getWidth() const { return mWindowWidth; }
		int getHeight() const { return mWindowHeight; }

	private:
		Display& operator=(const Display&);

		const int mWindowWidth;
		const int mWindowHeight;
	};
}

#endif