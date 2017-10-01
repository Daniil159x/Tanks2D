#include "mapfield.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>

#include <QFile>

#include "enums.hpp"

#include <fstream>


// example json


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

    if(!m_root["pathField"].isString()) {
        throw std::invalid_argument("MapField() - parse json error: 'pathField' is not string");
    }
    // TODO: добавить во все файлы префикс
    m_fileField = m_prefixFile + m_root["pathField"].toString();

    std::ifstream in_file(m_fileField.toStdString());
    if(!in_file.is_open()) {
        throw std::invalid_argument("MapField() - parse json error: 'pathField' is uncorrect or file is not opened");
    }
    std::string str;

    std::getline(in_file, str);
    auto len = str.length();
    do {
        if(len != str.length()){
            throw  std::invalid_argument("MapField() - file field is bad");
        }
        m_field += QString::fromStdString(str);
    } while (std::getline(in_file, str));
    m_fieldSize = { static_cast<int>(len), m_field.size() };



//    if(!m_root["size"].isArray()) {
//        throw std::invalid_argument("MapField() - parse json error: 'size' is not array of Int");
//    }
    if(!m_root["sizeSprite"].isArray()) {
        throw std::invalid_argument("MapField() - parse json error: 'size' is not array of Int");
    }

//    auto arr_size = m_root["size"].toArray();
    auto arr_sizeSprite = m_root["sizeSprite"].toArray();

//    if(arr_size.size() != 2) {
//        throw std::invalid_argument("MapField() - parse json error: 'size' is not array of Int)");
//    }
    if(arr_sizeSprite.size() != 2) {
        throw std::invalid_argument("MapField() - parse json error: 'sizeSprite' is not array of Int");
    }

//    m_fieldSize = {
//        arr_size[0].toInt(-1),
//        arr_size[1].toInt(-1)
//    };
//    if(m_fieldSize.width() == -1 || m_fieldSize.height() == -1) {
//        throw std::invalid_argument("MapField() - parse json error: 'size' is not array of Int)");
//    }

    m_spriteSize = {
        arr_sizeSprite[0].toInt(-1),
        arr_sizeSprite[1].toInt(-1)
    };
    if(m_spriteSize.width() == -1 || m_spriteSize.height() == -1) {
        throw std::invalid_argument("MapField() - parse json error: 'sizeSprite' is not array of Int)");
    }

    this->parseJson();
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
    else {
        throw std::invalid_argument("strToType() - parse json error: '" + str.toStdString() + "' is uncorrect type");
    }
}

std::vector<std::unique_ptr<Sprite> > MapField::getFiledSprites()
{
    std::vector<std::unique_ptr<Sprite> > vec_sprites;

    for(int i = 0; i < m_field.size(); ++i) {
        for(int j = 0; j < m_field.size(); ++j){
            if(!m_hashSprites.contains(m_field[i][j])){
                std::invalid_argument("getFiledSprites() - unknown sprite");
            }

            vec_sprites.push_back(std::make_unique<Sprite>(m_hashSprites[m_field[i][j]].m_vecImgs,
                                                           m_hashSprites[m_field[i][j]].m_size,
                                                           m_hashSprites[m_field[i][j]].m_type));
        }
    }

    return vec_sprites;
}

void MapField::parseJson()
{
    if(!m_root["cellName"].isArray()){
        throw std::invalid_argument("getSprite() - parse json error: 'cellName' is not array of charecter");
    }
    for(const auto &cellName : m_root["cellName"].toArray()){
        if(cellName.toString("").size() != 1){
            throw std::invalid_argument("getSprite() - parse json error: 'cellName' is not array of charecter");
        }

        if(!m_root[cellName.toString()].isObject()){
            throw std::invalid_argument("getSprite() - parse json error: '" + cellName.toString().toStdString()
                                                                            + "' has uncorrect format");
        }

        const auto &cell = m_root[cellName.toString()].toObject();
        typeItems cellType = strToType(cell["type"].toString());

        if(!cell["imgs"].isArray()){
            throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                              + " - 'imgs' is not array of object");
        }

        QVector<QImage> imgSprites;
        QRect rectSprite;
        imgSprites.reserve(cell["imgs"].toArray().size());

        for(const auto & img_obj : cell["imgs"].toArray()) {
            if(!img_obj.isObject()) {
                throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'imgs' is not array of object");
            }

            const auto &obj = img_obj.toObject();

            QImage sprite(obj["path"].toString());
            if(sprite.isNull()) {
                throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'imgs': can not open file");
            }
            if(!obj["pos"].isArray()) {
                throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' is not array of int");
            }
            const auto &arrayArgs = obj["pos"].toArray();
            if(arrayArgs.size() != 2) {
                throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' not contains 2 Int");
            }

            rectSprite = {
                arrayArgs[0].toInt(-1), arrayArgs[1].toInt(-1),
                m_spriteSize.width(), m_spriteSize.height()
            };
            if(rectSprite.x() == -1 || rectSprite.y() == -1){
                throw std::invalid_argument("getSprite() - parse json error: in " + cellName.toString().toStdString()
                                                                                  + " - 'pos' not contains 2 Int");
            }

            imgSprites.push_back(sprite.copy(rectSprite));
        }

        m_hashSprites.insert(cellName.toString()[0], {std::move(imgSprites), m_spriteSize, cellType});
    }
}

