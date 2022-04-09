#include <fstream> 
class Tarjeta
{
public:
	Tarjeta();
	~Tarjeta();
    unsigned int* buffer;

private: 

    unsigned int getpixel(int x, int y) {
        //assuming your x/y starts from top left, like I usually do
        return buffer[(64 - y) * 132 + x];
    }
};

Tarjeta::Tarjeta()
{
}

Tarjeta::~Tarjeta()
{
}
 