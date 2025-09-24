#ifndef GEN_H_
#define GEN_H_

#include <vector>

struct Color {
    // colour channels
    float r, g, b;

    // constructors
    Color();
    Color(float r, float g, float b);
    ~Color();
};

class Image {
  private:
    int _width;
    int _height;
    std::vector<Color> _colors;

  public:
    Image(int width, int height);
    ~Image();

    Color GetColor(int x, int y) const;
    void SetColor(const Color &, int x, int y);

    void Read(const char *path);
    void Export(const char *path) const;
};

#endif // GEN_H_
