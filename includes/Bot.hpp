#ifndef BOT_HPP
# define BOT_HPP

# include <string>
# include <cstdlib>

class Server;

inline std::string getJokeByIndex(int idx)
{
    static const char *jokes[5] = {
        "Neden yazilimcilar karanlik modu sever? Cunku isik bocekleri (bug) ceker! 🪲",
        "Dunyada 10 cesit insan vardir: Ikili sistemi (binary) anlayanlar ve anlamayanlar! 🤖",
        "Yazilimci markete gider. Esi 'Sut al, eger yumurta varsa 10 tane al' der. 10 kutu sutle doner! 🥛",
        "Bir yazilimcinin en buyuk yalani: 'Ama benim bilgisayarimda calisiyordu!' 💻",
        "Bir SQL sorgusu bara girer, iki masaya yaklasir ve sorar: 'Size katilabilir miyim? (JOIN)' 🍻"
    };
    return jokes[idx % 5];
}

inline std::string getRandomJoke()
{
    return getJokeByIndex(std::rand() % 5);
}

void broadcastBotJoke(Server *srv);

#endif
