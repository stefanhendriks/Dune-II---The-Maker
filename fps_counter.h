#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H
#include <SFML/Graphics.hpp>

class FPS_Counter : public sf::Drawable
{
public:
    FPS_Counter():
        mFont(),
        mStatisticsText(),
        mStatisticsUpdateTime(),
        mStatisticsNumFrames(0)
    {
        mFont.loadFromFile("DejaVuSans.ttf");
        mStatisticsText.setFont(mFont);
        mStatisticsText.setPosition(5.f, 5.f);
        mStatisticsText.setCharacterSize(10u);
    }

    void update(sf::Time dt){
        mStatisticsUpdateTime += dt;
        mStatisticsNumFrames += 1;
        if (mStatisticsUpdateTime >= sf::seconds(1.0f))
        {
            mStatisticsText.setString("FPS: " + std::to_string(mStatisticsNumFrames));

            mStatisticsUpdateTime -= sf::seconds(1.0f);
            mStatisticsNumFrames = 0;
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const{
        target.draw(mStatisticsText);
    }

private:
    sf::Font    mFont;
    sf::Text	mStatisticsText;
    sf::Time	mStatisticsUpdateTime;
    std::size_t	mStatisticsNumFrames;
};

#endif // FPS_COUNTER_H
