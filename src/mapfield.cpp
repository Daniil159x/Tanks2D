#include "mapfield.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>

#include <QFile>

#include "enums.hpp"

#include <fstream>

#include <QDebug>

MapField::MapField(const QString &fileName) : m_jsonFile(fileName), m_prefixFile(fileName.left(fileName.lastIndexOf('/')+1))
{
    QJsonParseError error;

    QFile file(m_jsonFile);
    file.open(QFile::OpenModeFlag::ReadOnly);
    m_root = QJsonDocument::fromJson(file.readAll(), &error).object();
    file.close();

    if(error.error != QJsonParseError::NoError) {
        throw std::invalid_argument("MapField() - read json error: " + error.errorString().toStdString());
    }

    this->parseDataField();
}

QSize MapField::fieldSize() const
{
    return m_fieldSize;
}

typeItems strToType(const QString &str) {
    if(str == "ignore") {
        return typeItems::ignoreCollize;
    }
    else if(str == "block"){
        return typeItems::block;
    }
    else if(str == "breakable") {
        return typeItems::breakable;
    }
    else if(str == "player") {
        return typeItems::player;
    }
    else {
        throw std::invalid_argument("strToType() - parse json error: '" + str.toStdString() + "' is uncorrect type");
    }
}

std::vector<std::unique_ptr<Sprite> > MapField::getFiledSprites() /*const*/
{
    std::vector<std::unique_ptr<Sprite> > vec_sprites;

    const auto w = m_spriteSize.width();
    const auto h = m_spriteSize.height();

    for(int i = 0; i < m_field.size(); ++i) {
        for(int j = 0; j < m_field[i].size(); ++j){
            QChar sp = m_field[i][j];
            if(sp == 'p' || sp == 'P') {
                // TODO: скорее всего костыль, передалать
                if(sp == 'p'){
                    m_dataPlayers[0].m_pos = {j, i};
                }
                else {
                    m_dataPlayers[1].m_pos = {j, i};
                }

                // NOTE: предполагается, что B - это background
                sp = 'B';
            }

            if(!m_hashSprites.contains(sp)){
                std::invalid_argument("getFiledSprites() - unknown sprite");
            }

            vec_sprites.push_back(std::make_unique<Sprite>(m_hashSprites[sp].m_vecImgs,
                                                           m_hashSprites[sp].m_size,
                                                           m_hashSprites[sp].m_type));
            vec_sprites.back()->setPos(w*j, h*i);
            qDebug() << i << j;
        }
    }

    return vec_sprites;
}

void MapField::parseDataField()
{
    /*
     * ВАРНИНГ! НЕЧИТАБИЛЬНЫЙ КОД
     */

    m_nameMap = m_root["NameMap"].toString();

    if(!m_root["sizeSprite"].isArray()) {
        throw std::invalid_argument("parseDataField() - parse json error: 'size' is not array of Int");
    }

    auto arr_sizeSprite = m_root["sizeSprite"].toArray();

    if(arr_sizeSprite.size() != 2) {
        throw std::invalid_argument("parseDataField() - parse json error: 'sizeSprite' is not array of Int");
    }
    m_spriteSize = {
        arr_sizeSprite[0].toInt(-1),
        arr_sizeSprite[1].toInt(-1)
    };
    if(m_spriteSize.width() == -1 || m_spriteSize.height() == -1) {
        throw std::invalid_argument("parseDataField() - parse json error: 'sizeSprite' is not array of Int)");
    }


    if(!m_root["pathField"].isString()) {
        throw std::invalid_argument("parseDataField() - parse json error: 'pathField' is not string");
    }

    m_fileField = m_prefixFile + m_root["pathField"].toString();

    std::ifstream in_file(m_fileField.toStdString());
    if(!in_file.is_open()) {
        throw std::invalid_argument("parseDataField() - parse json error: 'pathField' is uncorrect or file is not opened");
    }
    std::string str;


    int count_playerField[2] = { 0 };
    std::getline(in_file, str);
    auto len = str.length();
    do {
        if(len != str.length()){
            throw  std::invalid_argument("parseDataField() - file field is bad");
        }
        m_field += QString::fromStdString(str);

        count_playerField[0] += (str.find('p') != std::string::npos);
        count_playerField[1] += (str.find('P') != std::string::npos);

    } while (std::getline(in_file, str));
    m_fieldSize = { static_cast<int>(len) * m_spriteSize.width(), m_field.size() * m_spriteSize.height() };

    if(count_playerField[0] != 1 || count_playerField[1] != 1){
        throw  std::invalid_argument("parseDataField() - file read: player fields uncorrect");
    }

    if(!m_root["cellName"].isArray()){
        throw std::invalid_argument("parseDataField() - parse json error: 'cellName' is not array of charecter");
    }

    // find info about players
    if(!m_root["cellName"].toArray().contains("P") || !m_root.contains("P")) {
        throw std::invalid_argument("parseDataField() - parse json error: 'P' not found");
    }
    if(!m_root["cellName"].toArray().contains("p") || !m_root.contains("p")) {
        throw std::invalid_argument("parseDataField() - parse json error: 'p' not found");
    }


    if(!m_root.contains("bullet")) {
        throw std::invalid_argument("parseDataField() - parse json error: 'bullet' not found");
    }
    if(!m_root["bullet"].isArray()) {
        throw std::invalid_argument("parseDataField() - parse json error: 'bullet' has uncorrect format");
    }
    for(const auto &bullet_el : m_root["bullet"].toArray()){
        QRect rectSprite;
        if(!bullet_el.isObject()) {
            throw std::invalid_argument("parseDataField() - 'bullet' has uncorrect format");
        }

        const auto &obj = bullet_el.toObject();

        QImage sprite(m_prefixFile + obj["path"].toString());
        if(sprite.isNull()) {
            throw std::invalid_argument("parseDataField() - parse json error: in 'bullet' - 'path': can not open file");
        }
        if(!obj["pos"].isArray()) {
            throw std::invalid_argument("parseDataField() - parse json error: in 'bullet' - 'pos' is not array of int");
        }
        const auto &arrayArgs = obj["pos"].toArray();
        if(arrayArgs.size() != 2) {
            throw std::invalid_argument("parseDataField() - parse json error: in in 'bullet' - 'pos' not contains 2 Int");
        }

        rectSprite = {
            arrayArgs[0].toInt(-1), arrayArgs[1].toInt(-1),
            m_spriteSize.width(), m_spriteSize.height()
        };
        if(rectSprite.x() == -1 || rectSprite.y() == -1){
            throw std::invalid_argument("parseDataField() - parse json error: in in 'bullet' - 'pos' not contains 2 Int");
        }

        m_BulletImages.push_back(sprite.copy(rectSprite));
    }

    for(const auto &cellName : m_root["cellName"].toArray()){
        if(cellName.toString("").size() != 1){
            throw std::invalid_argument("parseDataField() - parse json error: 'cellName' is not array of charecter");
        }

        if(!m_root[cellName.toString()].isObject()){
            throw std::invalid_argument("parseDataField() - parse json error: '" + cellName.toString().toStdString()
                                                                            + "' has uncorrect format");
        }

        const auto &cell = m_root[cellName.toString()].toObject();
        typeItems cellType = strToType(cell["type"].toString());

        if(!cell["imgs"].isArray()){
            throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                              + " - 'imgs' is not array of object");
        }

        QVector<QImage> imgSprites;
        QRect rectSprite;
        imgSprites.reserve(cell["imgs"].toArray().size());

        for(const auto & img_obj : cell["imgs"].toArray()) {
            if(!img_obj.isObject()) {
                throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'imgs' is not array of object");
            }

            const auto &obj = img_obj.toObject();

            QImage sprite(m_prefixFile + obj["path"].toString());
            if(sprite.isNull()) {
                throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'imgs': can not open file");
            }
            if(!obj["pos"].isArray()) {
                throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' is not array of int");
            }
            const auto &arrayArgs = obj["pos"].toArray();
            if(arrayArgs.size() != 2) {
                throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' not contains 2 Int");
            }

            rectSprite = {
                arrayArgs[0].toInt(-1), arrayArgs[1].toInt(-1),
                m_spriteSize.width(), m_spriteSize.height()
            };
            if(rectSprite.x() == -1 || rectSprite.y() == -1){
                throw std::invalid_argument("parseDataField() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' not contains 2 Int");
            }

            imgSprites.push_back(sprite.copy(rectSprite));
        }
        if(cellName.toString()[0] == 'p'){
            // TODO: сделать рефакторинг этого куска кода
            QMatrix mx_rotate;
            mx_rotate.rotate(90);

            // NO - destroyed
            imgSprites.push_front(QImage());
            // right
            imgSprites.push_back(imgSprites.last().transformed(mx_rotate));
            // down
            imgSprites.push_back(imgSprites.last().transformed(mx_rotate));
            // left
            imgSprites.push_back(imgSprites.last().transformed(mx_rotate));
            m_dataPlayers[0].m_data = {std::move(imgSprites), m_spriteSize, cellType};
        }
        else if(cellName.toString()[0] == 'P'){
            QMatrix mx_rotate;
            mx_rotate.rotate(90);
            // NO - destroyed
            imgSprites.push_front(QImage());
            // right
            imgSprites.push_back( imgSprites.last().transformed(mx_rotate));
            // down
            imgSprites.push_back( imgSprites.last().transformed(mx_rotate));
            // left
            imgSprites.push_back( imgSprites.last().transformed(mx_rotate));
            m_dataPlayers[1].m_data = {std::move(imgSprites), m_spriteSize, cellType};
        }
        else {
            m_hashSprites.insert(cellName.toString()[0], {std::move(imgSprites), m_spriteSize, cellType});
        }
    }
}

QSize MapField::getFieldSize() const
{
    return m_fieldSize;
}

const QVector<QImage> &MapField::getBulletImages() const
{
    return m_BulletImages;
}

QString MapField::getNameMap() const
{
    return m_nameMap;
}

std::tuple<std::unique_ptr<PlayerSprite>, std::unique_ptr<PlayerSprite> > MapField::getPlayers() const
{
    const auto w = m_spriteSize.width();
    const auto h = m_spriteSize.height();

    auto pl1 = std::make_unique<PlayerSprite>(m_dataPlayers[0].m_data.m_vecImgs,
                                              m_dataPlayers[0].m_data.m_size);
    pl1->setPos(w*m_dataPlayers[0].m_pos.x(), h*m_dataPlayers[0].m_pos.y());

    auto pl2 = std::make_unique<PlayerSprite>(m_dataPlayers[1].m_data.m_vecImgs,
                                              m_dataPlayers[1].m_data.m_size);
    pl2->setPos(w*m_dataPlayers[1].m_pos.x(), h*m_dataPlayers[1].m_pos.y());

    return { std::move(pl1), std::move(pl2) };
}
