#include "mapfield.hpp"

#include "sprite.hpp"
#include "playersprite.hpp"

#include <QDebug>

#include <cassert>
#include <fstream>
#include <memory>

#define PL1 'p'
#define PL2 'P'

//int indexOfDir(motion_vector d){
//    switch (d) {
//        case motion_vector::No:    return 0;
//        case motion_vector::Up:    return 1;
//        case motion_vector::Left:  return 4;
//        case motion_vector::Down:  return 3;
//        case motion_vector::Right: return 2;
//        default:
//            throw std::logic_error("indexOfDir: default branch");
//    }
//}

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
        typeItems::ignoreCollize, m_rect
    };


    // block
    m_hash['Q'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block___.bmp").copy(0, 0, 32, 32)
        },
        typeItems::block, m_rect
    };

    // 'D' - destructible
    m_hash['D'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(0, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(32, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_02.bmp").copy(64, 0, 32, 32)
        },
        typeItems::breakable, m_rect
    };

    // 'I' - ....
    m_hash['I'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_01.bmp").copy(0, 0, 32, 32),
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/block_01.bmp").copy(32, 0, 32, 32)
        },
        typeItems::breakable, m_rect
    };

    // 'p' - first player
    m_hash[PL1] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_1.png")
        },
        typeItems::player, {}
    };
    m_hash[PL1].m_rect = m_hash[PL1].m_vec.first().rect();

    // 'P' - second player
    m_hash[PL2] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/player_2.png")
        },
        typeItems::player, {}
    };
    m_hash[PL2].m_rect = m_hash[PL2].m_vec.first().rect();

    // 'b' - bullet
    m_hash['b'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/bullet02.png")
        },
        typeItems::bullet, {}
    };
    m_hash['b'].m_rect = m_hash['b'].m_vec.first().rect();

    m_background = QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/ground.bmp").copy(0, 0, 32, 32);

}

std::vector<std::unique_ptr<Sprite> > MapField::getFiledSprites() const
{
    std::vector<std::unique_ptr<Sprite> > res;
    res.reserve(static_cast<size_t>(m_field.size()*m_field[0].size()));

    for(int i = 0; i < m_field.size(); ++i){
        for(int j = 0; j < m_field[i].size(); ++j){
            std::unique_ptr<Sprite> ptr;
            QChar Spr;
            if(m_field[i][j] == PL1 || m_field[i][j] == PL2){
                Spr = 'B';
            }
            else {
                Spr = m_field[i][j];
            }
            ptr = std::make_unique<Sprite>(*this, QSize{m_hash[Spr].m_rect.width(), m_hash[Spr].m_rect.height() },
                                                        Spr, m_hash[Spr].m_type);
            ptr->setPos(j*m_rect.width(), i*m_rect.height());
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

    auto ptr_1 = std::make_unique<PlayerSprite>(*this, QSize{m_hash[PL1].m_rect.width(), m_hash[PL1].m_rect.height()}, PL1);
    ptr_1->setPos(pos_pl1);
    ptr_1->setZValue(5);

    auto ptr_2 = std::make_unique<PlayerSprite>(*this, QSize{m_hash[PL2].m_rect.width(), m_hash[PL2].m_rect.height()}, PL2);
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

const QImage &MapField::getImage_forPlayer(QChar Spr) const
{
    assert(m_hash.contains(Spr));
    return m_hash[Spr].m_vec[0];
}

const QImage &MapField::getImage_forBullet() const
{
    return m_hash['b'].m_vec[0];
}

const QImage &MapField::getImage_forEffect(int i, const QImage &default_img) const
{
    return (i < m_effect.size()) ? m_effect[i] : default_img;
}

int MapField::getSize_effect() const
{
    return m_effect.size();
}

int MapField::getSizeImage_forSprites(QChar Spr) const
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

QSize MapField::getSize_bullet() const
{
    return {m_hash['b'].m_rect.width(), m_hash['b'].m_rect.height()};
}

const QImage &MapField::getImage_background() const
{
    return MapField::m_background;
}
