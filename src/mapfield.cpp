#include "mapfield.hpp"

#include "sprite.hpp"
#include "playersprite.hpp"

#include <QDebug>

#include <cassert>
#include <fstream>
#include <memory>

#define PL1 'p'
#define PL2 'P'
#define BACK 'B'
#define BULLET 'b'


#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

#include <QDir>

#define MY_ASSERT(expr, where, what) \
    if(!(expr)) { throw std::logic_error(QString("[%1] {%2} - %3").arg(where).arg(__LINE__).arg(what).toStdString() ); }


MapField::MapField(const QString &fileName) : m_pl1_CellName(PL1), m_pl2_CellName(PL2), m_backgroundCellName(BACK), m_bulletCellName(BULLET)
{


    Q_UNUSED(fileName);

    QFile jsonFile(fileName);
    jsonFile.open(QFile::OpenModeFlag::ReadOnly);
    MY_ASSERT(jsonFile.isOpen(), "MapField::MapField()", "file is not open");


    QDir dir(fileName);
    m_prefix = dir.absolutePath().left(dir.absolutePath().lastIndexOf('/') + 1);


    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
    m_root = doc.object();
    MY_ASSERT(!m_root.isEmpty(), "MapField::MapField()", "json error parse: " + error.errorString());
    jsonFile.close();

    /// load rect sprites
    this->loadRectSprites();

    /// load field
    this->loadField();

    /// load cell names
    auto cells = this->getStringOfCellSprite();
    for(auto i : cells) {
        this->loadCellSprite(i);
    }

    /// set player1
    auto player_1_cell = m_root["player_1"].toString();
    // custom cell
    // default cell == PL1
    if(player_1_cell.size() == 1) {
        m_pl1_CellName = player_1_cell[0];
    }
    MY_ASSERT(m_hash.contains(m_pl1_CellName),
              "MapField::MapField()", QString("player1 as '%1' not found").arg(m_pl1_CellName));
    m_hash[m_pl1_CellName].m_type = typeItems::player;
    m_hash[m_pl1_CellName].m_rect = m_hash[m_pl1_CellName].m_vec.first().rect();


    /// set player2
    auto player_2_cell = m_root["player_2"].toString();
    // custom cell
    // default cell == PL2
    if(player_2_cell.size() == 1) {
        m_pl2_CellName = player_2_cell[0];
    }
    MY_ASSERT(m_hash.contains(m_pl2_CellName),
              "MapField::MapField()", QString("player2 as '%1' not found").arg(m_pl2_CellName));
    m_hash[m_pl2_CellName].m_type = typeItems::player;
    m_hash[m_pl2_CellName].m_rect = m_hash[m_pl2_CellName].m_vec.first().rect();


    /// set background
    auto background_cell = m_root["background"].toString();
    // custom cell
    // default cell == BACK
    if(background_cell.size() == 1) {
        m_backgroundCellName = background_cell[0];
    }

    m_background = m_hash[m_backgroundCellName].m_vec.first();
    MY_ASSERT(m_hash.contains(m_backgroundCellName),
              "MapField::MapField()", QString("background as '%1' not found").arg(m_backgroundCellName));
    MY_ASSERT((m_background.size() == QSize{m_rect.width(), m_rect.height()}),
              "MapField::MapField()", QString("background as '%1' has discrepancy with RectSprites").arg(m_backgroundCellName));



    /// set bullet
    auto bullet_cell = m_root["bullet"].toString();
    if(bullet_cell.size() == 1) {
        m_bulletCellName = bullet_cell[0];
    }
    MY_ASSERT(m_hash.contains(m_bulletCellName),
              "MapField::MapField()", QString("bullet as '%1' not found").arg(m_bulletCellName));
    m_hash[m_bulletCellName].m_type = typeItems::bullet;
    m_hash[m_bulletCellName].m_rect = m_hash[m_bulletCellName].m_vec.first().rect();


    /// set effects
    // if has effects, then load, else the game without the effects
    if(m_root["effects"].isArray()) {
        const auto & effects_arr = m_root["effects"].toArray();
        for(const auto &i : effects_arr) {
            const auto &obj = i.toObject();

            const auto &path    = obj["path"].toString();
            const auto &rect_arr = obj["rect"].toArray();

            const int x = rect_arr[0].toInt(-1);
            const int y = rect_arr[1].toInt(-1);

            const int w = rect_arr[2].toInt(-1);
            const int h = rect_arr[3].toInt(-1);

            MY_ASSERT( (!path.isEmpty() && x != -1 && y != -1 && h != -1 && w != -1),
                       "MapField::MapField", "effect image is incorrect");

            m_effect.push_back( QImage(m_prefix + path).copy(x, y, w, h)  );
        }
    }


/*
    // 'B' - background
    m_hash['B'] = {
        {
            QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/ground.bmp").copy(0, 0, 32, 32)
        },
        typeItems::ignoreCollision, m_rect
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


    // effect
    for(int i = 0; i < 15; ++i){
        m_effect.push_back(QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/sprite_sheet.png").copy(20*i, 0, 20, 20));
    }

    m_background = QImage("/home/daniil159x/Qt_project/Tanks2D/maps/test/ground.bmp").copy(0, 0, 32, 32); */

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
                Spr = m_backgroundCellName;
            }
            else {
                Spr = m_field[i][j];
            }
            ptr = std::make_unique<Sprite>(*this, Spr, m_hash[Spr].m_type);
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
        int p1 = m_field[i].indexOf(m_pl1_CellName);
        int p2 = m_field[i].indexOf(m_pl2_CellName);

        if(p1 != -1){
            pos_pl1 = {p1*m_rect.width(), i*m_rect.height()};
        }
        if(p2 != -1){
            pos_pl2 = {p2*m_rect.width(), i*m_rect.height()};
        }
    }

    auto ptr_1 = std::make_unique<PlayerSprite>(*this, m_pl1_CellName);
    ptr_1->setPos(pos_pl1);
    ptr_1->setZValue(5);

    auto ptr_2 = std::make_unique<PlayerSprite>(*this, m_pl2_CellName);
    ptr_2->setPos(pos_pl2);
    ptr_2->setZValue(5);

    res.push_back(std::move(ptr_1));
    res.push_back(std::move(ptr_2));

    return res;
}

const QImage& MapField::getImage_forSprite(int i, QChar cellName, const QImage &default_img) const
{
    MY_ASSERT(m_hash.contains(cellName), "MapField::getImage_forSprite", QString("'%1' is not registered").arg(cellName));
    return (i < m_hash[cellName].m_vec.size()) ? m_hash[cellName].m_vec[i] : default_img;
}

const QImage &MapField::getImage_forPlayer(QChar cellName) const
{
    MY_ASSERT(m_hash.contains(cellName), "MapField::getImage_forPlayer", QString("'%1' is not registered").arg(cellName));
    return m_hash[cellName].m_vec[0];
}

const QImage &MapField::getImage_forBullet() const
{
    MY_ASSERT(m_hash.contains(m_bulletCellName), "MapField::getImage_forSprite", QString("'%1' is not registered").arg(m_bulletCellName));
    return m_hash[m_bulletCellName].m_vec[0];
}

const QImage &MapField::getImage_forEffect(int i, const QImage &default_img) const
{
    return (i < m_effect.size()) ? m_effect[i] : default_img;
}

int MapField::getSize_effect() const
{
    return m_effect.size();
}

int MapField::getSizeImage_forSprites(QChar cellName) const
{
    MY_ASSERT(m_hash.contains(cellName), "MapField::getSizeImage_forSprites", QString("'%1' is not registered").arg(cellName));
    return m_hash[cellName].m_vec.size();
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
    MY_ASSERT(m_hash.contains(m_bulletCellName), "MapField::getSize_bullet", QString("'%1' is not registered").arg(m_bulletCellName));
    return {m_hash[m_bulletCellName].m_rect.width(), m_hash[m_bulletCellName].m_rect.height()};
}

const QImage &MapField::getImage_background() const
{
    return MapField::m_background;
}

QChar MapField::getPl1CellName() const
{
    return m_pl1_CellName;
}

QChar MapField::getPl2CellName() const
{
    return m_pl2_CellName;
}

QChar MapField::getBackgroundCellName() const
{
    return m_backgroundCellName;
}

QChar MapField::getBulletCellName() const
{
    return m_bulletCellName;
}

void MapField::loadField()
{
    const auto &file_field = m_root["file_field"].toString();
    MY_ASSERT(!file_field.isEmpty(), "MapField::loadField()", "file_field not found");

    std::ifstream in_file((m_prefix + file_field).toStdString());
    MY_ASSERT(in_file.is_open(), "MapField::loadField()", "file field is not open");

    std::string str;

    int count_playerField[2] = { 0 };
    std::getline(in_file, str);
    auto len = str.length();
    do {
        MY_ASSERT(len == str.length(), "MapField::loadField()", "file field is bad(disproportionate)");
        m_field += QString::fromStdString(str);

        count_playerField[0] += (str.find(PL1) != std::string::npos);
        count_playerField[1] += (str.find(PL2) != std::string::npos);

    } while (std::getline(in_file, str));

    MY_ASSERT(count_playerField[0] == 1, "MapField::loadField()", "player coordinates is uncorrect");
    MY_ASSERT(count_playerField[1] == 1, "MapField::loadField()", "player coordinates is uncorrect");

    m_filedSize = { m_field[0].size() * m_rect.width(), m_field.size() * m_rect.height() };
    qDebug() << "field size ==" << m_filedSize;
}

void MapField::loadRectSprites()
{
    const auto &rectArr = m_root["size_sprites"].toArray();
    MY_ASSERT(rectArr.size() == 2, "MapField::loadRectSprites()", "size_sprites is not array");
    const int w = rectArr[0].toInt(-1);
    const int h = rectArr[1].toInt(-1);

    MY_ASSERT(w != -1 && h != -1, "MapField::loadRectSprites()", "size_sprites is incorrect");
    m_rect = {0, 0, w, h};
}

QString MapField::getStringOfCellSprite() const
{
    if(m_root["cellName"].isString()) {
        return m_root["cellName"].toString();
    }
    else if(m_root["cellName"].isArray()) {
        const auto &cell_arr = m_root["cellName"].toArray();
        QString res;
        for(const auto &i : cell_arr) {
            MY_ASSERT(i.toString().size() == 1, "MapField::getStringOfCellSprite", "cellName has incorrect format");
            res += i.toString()[0];
        }
        return res;
    }
    else {
        MY_ASSERT(false, "MapField::getStringOfCellSprite()", "cellName is not array or string");
    }
}


typeItems getType(const QString &s) {
    if(s == "ignore") {
        return typeItems::ignoreCollision;
    }
    else if(s == "breakable") {
        return typeItems::breakable;
    }
    else if(s == "block") {
        return typeItems::block;
    }
    else {
        return typeItems::ignoreCollision;
    }
}

void MapField::loadCellSprite(QChar ch)
{
    const auto &obj = m_root[ch].toObject();
    MY_ASSERT(!obj.isEmpty(), "MapField::loadCellSprite()", QString("sprite('%1') has incorrect format").arg(ch));

    // type sprite
    typeItems type;
    if(ch == PL1 || ch == PL2) {
        type = typeItems::player;
    }
    else {
        const auto &type_str = obj["type"].toString();
        type = getType(type_str);
    }

    const auto &size_arr = obj["size"].toArray({-1, -1});
    // rect sprite
    QRect rectSprite = m_rect;
    if(size_arr.size() == 2 && size_arr != QJsonArray({-1, -1}) ) {
        const auto w = size_arr[0].toInt(-1);
        const auto h = size_arr[1].toInt(-1);
        if(w != -1 && h != -1){
            rectSprite = {0, 0, w, h};
        }
    }



    // img sprite
    const auto &objImg_arr = obj["imgs"].toArray();
    MY_ASSERT(!objImg_arr.isEmpty(), "MapField::loadCellSprite()", QString("'img' of sprite('%1') has incorrect format").arg(ch));

    m_hash[ch] = { QVector<QImage>{ objImg_arr.size() }, type, rectSprite };


    for(int i = 0; i < objImg_arr.size(); ++i) {
        const auto &objImg = objImg_arr[i].toObject();

        const auto &pathImg      = objImg["path"].toString();
        const auto &pointImg_arr = objImg["pos"].toArray();
        const int x = pointImg_arr[0].toInt(-1);
        const int y = pointImg_arr[1].toInt(-1);

        MY_ASSERT((!pathImg.isEmpty() && x != -1 && y != -1),
                                "MapField::loadCellSprite()",
                                QString("'img'[%1] of sprite('%2') invalid").arg(i).arg(ch));

        m_hash[ch].m_vec[i] = QImage(m_prefix + pathImg).copy(x, y, rectSprite.width(), rectSprite.height());

        MY_ASSERT(!m_hash[ch].m_vec[i].isNull(), "MapField::loadCellSprite", QString("'img'[%1] of sprite('%2') invalid").arg(i).arg(ch));
    }
}
