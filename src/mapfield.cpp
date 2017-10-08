#include "mapfield.hpp"

#include "sprite.hpp"
#include "playersprite.hpp"

#include <QDebug>

#include <cassert>
#include <fstream>
#include <memory>

#define PL1 'p'
#define PL2 'P'

int indexOfDir(dir d){
    switch (d) {
        case dir::No:    return 0;
        case dir::Up:    return 1;
        case dir::Left:  return 4;
        case dir::Down:  return 3;
        case dir::Right: return 2;
        default:
            throw std::logic_error("indexOfDir: default branch");
    }
}

//QRect operator * (const QRect& rect, qreal multi) {
//    return { static_cast<int>(rect.x() * multi),
//             static_cast<int>(rect.y() * multi),
//             static_cast<int>(rect.width() * multi),
//             static_cast<int>(rect.height() * multi) };
//}

MapField::MapField(const QString &fileName)
{
    Q_UNUSED(fileName);
    // TODO: сделать кастомизацию


    std::ifstream in_file("/home/daniil159x/Qt_project/Tanks2D/maps/test/field.txt");
    std::string str;

    int count_playerField[2] = { 0 };
    std::getline(in_file, str);
    auto len = str.length();
    do {
        if(len != str.length()){
            throw  std::invalid_argument("MapField() - file field is bad");
        }
        m_field += QString::fromStdString(str);

        count_playerField[0] += (str.find(PL1) != std::string::npos);
        count_playerField[1] += (str.find(PL2) != std::string::npos);

    } while (std::getline(in_file, str));
    m_filedSize = { m_field[0].size() * m_rect.width(), m_field.size() * m_rect.height() };
    qDebug() << "field size ==" << m_filedSize;


    // 'B' - background
    m_hash['B'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/ground.bmp").copy(0, 0, 32, 32)
        },
        typeItems::ignoreCollize
    };


    // block
    m_hash['Q'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block___.bmp").copy(0, 0, 32, 32)
        },
        typeItems::block
    };

    // 'D' - destructible
    m_hash['D'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(0, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(32, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(64, 0, 32, 32)
        },
        typeItems::breakable
    };

    // 'I' - ....
    m_hash['I'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_01.bmp").copy(0, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_01.bmp").copy(32, 0, 32, 32)
        },
        typeItems::breakable
    };

    // 'p' - first player
    QMatrix mx_90;
    mx_90.rotate(90);
    QMatrix mx_180;
    mx_180.rotate(180);
    QMatrix mx_270;
    mx_270.rotate(270);
    m_hash[PL1] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/noImg.png"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_1.bmp"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_1.bmp").transformed(mx_90),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_1.bmp").transformed(mx_180),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_1.bmp").transformed(mx_270)
        },
        typeItems::player
    };

    // 'P' - second player
    m_hash[PL2] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/noImg.png"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_2.bmp"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_2.bmp").transformed(mx_90),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_2.bmp").transformed(mx_180),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_2.bmp").transformed(mx_270)
        },
        typeItems::player
    };

    // 'b' - bullet
    m_hash['b'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/noImg.png"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/bullet02.png"),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/bullet02.png").transformed(mx_90),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/bullet02.png").transformed(mx_180),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/bullet02.png").transformed(mx_270)
        },
        typeItems::bullet
    };

    m_background = QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/ground.bmp").copy(0, 0, 32, 32);

}

std::vector<std::unique_ptr<Sprite> > MapField::getFiledSprites() const
{
    std::vector<std::unique_ptr<Sprite> > res;
    res.reserve(static_cast<size_t>(m_field.size()*m_field[0].size()));

    for(int i = 0; i < m_field.size(); ++i){
        for(int j = 0; j < m_field[i].size(); ++j){
            std::unique_ptr<Sprite> ptr;
            if(m_field[i][j] == PL1 || m_field[i][j] == PL2){
                ptr = std::make_unique<Sprite>(*this, QSize{32, 32}, 'B', m_hash['B'].m_type);
            }
            else {
                ptr = std::make_unique<Sprite>(*this, QSize{32, 32}, m_field[i][j], m_hash[m_field[i][j]].m_type);
            }
            ptr->setPos(j*32, i*32);
            ptr->setZValue(1);
            res.push_back(std::move(ptr));
        }
    }
    return res;
}

std::vector<std::unique_ptr<PlayerSprite> > MapField::getPlayerSprites() const
{
    std::vector<std::unique_ptr<PlayerSprite> > res;
    res.reserve(2);

    QPoint pos_pl1, pos_pl2;
    for(int i = 0; i < m_field.size(); ++i) {
        int p1 = m_field[i].indexOf(PL1);
        int p2 = m_field[i].indexOf(PL2);

        if(p1 != -1){
            pos_pl1 = {p1*32, i*32};
        }
        if(p2 != -1){
            pos_pl2 = {p2*32, i*32};
        }
    }

    auto ptr_1 = std::make_unique<PlayerSprite>(*this, QSize{32, 32}, PL1);
    ptr_1->setPos(pos_pl1);
    ptr_1->setZValue(5);

    auto ptr_2 = std::make_unique<PlayerSprite>(*this, QSize{32, 32}, PL2);
    ptr_2->setPos(pos_pl2);
    ptr_2->setZValue(5);

    res.push_back(std::move(ptr_1));
    res.push_back(std::move(ptr_2));

    return res;
}

const QImage& MapField::getImage_forSprite(int i, QChar Spr, const QImage &default_img) const
{
    assert(m_hash.contains(Spr));
    return (i < m_hash[Spr].m_vec.size()) ? m_hash[Spr].m_vec[i] : default_img;
}

const QImage &MapField::getImage_forPlayer(dir d, QChar Spr) const
{
    assert(m_hash.contains(Spr));
    return m_hash[Spr].m_vec[indexOfDir(d)];
}

const QImage &MapField::getImage_forBullet(dir d) const
{
    return m_hash['b'].m_vec[indexOfDir(d)];
}

const QImage &MapField::getImage_forEffect(int i, const QImage &default_img) const
{
    return (i < m_effect.size()) ? m_effect[i] : default_img;
}

int MapField::getSize_effect() const
{
    return m_effect.size();
}

int MapField::getSize_forSprites(QChar Spr) const
{
    assert(m_hash.contains(Spr));
    return m_hash[Spr].m_vec.size();
}

QSize MapField::getSize_field() const
{
    return m_filedSize;
}

QSize MapField::getSize_sprites() const
{
    return {m_rect.width(), m_rect.height()};
}

const QImage &MapField::getImage_background() const
{
    return MapField::m_background;
}
