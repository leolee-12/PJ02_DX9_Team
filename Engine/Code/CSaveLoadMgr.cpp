#include "CSaveLoadMgr.h"
#include <iostream>

IMPLEMENT_SINGLETON(CSaveLoadMgr)

CSaveLoadMgr::CSaveLoadMgr()
{
}

CSaveLoadMgr::~CSaveLoadMgr()
{
}

void CSaveLoadMgr::Ready_SaveLoad()
{
    MapSet.clear();

    std::ifstream mapStringsFile("mapStrings.txt");
    if (!mapStringsFile.is_open())
    {
        std::ofstream createFile("mapStrings.txt");
        createFile.close();
        return;
    }

    std::string mapName;

    // mapStrings.txt에서 한 줄씩 맵 이름 읽기
    while (std::getline(mapStringsFile, mapName))
    {
        // 빈 줄 스킵
        if (mapName.empty())
            continue;

        // JSON 파일 경로 생성
        std::string jsonFilePath = mapName + ".json";
        std::ifstream jsonFile(jsonFilePath);

        if (!jsonFile.is_open())
        {
            std::cerr << "Failed to open JSON file: " << jsonFilePath << std::endl;
            continue;
        }

        // JSON 파싱
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;

        if (Json::parseFromStream(builder, jsonFile, &root, &errs))
        {
            // MapSet에 저장 (맵 이름, JSON 데이터)
            MapSet.push_back(std::make_pair(mapName, root));
            std::cout << "Loaded map: " << mapName << std::endl;
        }
        else
        {
            std::cerr << "Failed to parse JSON file: " << jsonFilePath << std::endl;
            std::cerr << "Error: " << errs << std::endl;
        }

        jsonFile.close();
    }

    mapStringsFile.close();

    std::cout << "Ready_SaveLoad completed. Loaded " << MapSet.size() << " maps." << std::endl;
}

void CSaveLoadMgr::Save(string fileName)
{
    // 1. JSON 파일로 저장
    std::string jsonFilePath = fileName + ".json";

    // 현재 MapSet에서 해당 맵 찾기 (업데이트용)
    Json::Value dataToSave;
    bool found = false;

    for (auto& mapData : MapSet)
    {
        if (mapData.first == fileName)
        {
            dataToSave = mapData.second;
            found = true;
            break;
        }
    }

    // 찾지 못했으면 새로운 JSON 생성
    if (!found)
    {
        dataToSave = Json::Value(Json::objectValue);
        // 필요한 기본 구조 초기화
        dataToSave["MapName"] = fileName;
        dataToSave["Objects"] = Json::Value(Json::arrayValue);
    }

    // JSON 파일 쓰기
    std::ofstream jsonFile(jsonFilePath);

    if (!jsonFile.is_open())
    {
        std::cerr << "Failed to create JSON file: " << jsonFilePath << std::endl;
        return;
    }

    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "    "; // 4칸 들여쓰기
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    writer->write(dataToSave, &jsonFile);

    jsonFile.close();
    std::cout << "Saved JSON file: " << jsonFilePath << std::endl;

    // 2. mapStrings.txt에 파일명 추가 (중복 체크)

    // 먼저 기존 파일 읽어서 중복 확인
    std::ifstream mapStringsFileRead("mapStrings.txt");
    std::vector<std::string> existingMaps;
    bool alreadyExists = false;

    if (mapStringsFileRead.is_open())
    {
        std::string line;
        while (std::getline(mapStringsFileRead, line))
        {
            if (!line.empty())
            {
                existingMaps.push_back(line);
                if (line == fileName)
                {
                    alreadyExists = true;
                }
            }
        }
        mapStringsFileRead.close();
    }

    // 중복이 아니면 추가
    if (!alreadyExists)
    {
        std::ofstream mapStringsFileWrite("mapStrings.txt", std::ios::app);

        if (mapStringsFileWrite.is_open())
        {
            mapStringsFileWrite << fileName << std::endl;
            mapStringsFileWrite.close();
            std::cout << "Added map name to mapStrings.txt: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "Failed to open mapStrings.txt for writing" << std::endl;
        }

        // MapSet에도 추가
        if (!found)
        {
            MapSet.push_back(std::make_pair(fileName, dataToSave));
        }
    }
    else
    {
        std::cout << "Map name already exists in mapStrings.txt: " << fileName << std::endl;

        // MapSet 업데이트
        if (found)
        {
            for (auto& mapData : MapSet)
            {
                if (mapData.first == fileName)
                {
                    mapData.second = dataToSave;
                    break;
                }
            }
        }
    }

    std::cout << "Save completed: " << fileName << std::endl;
}

void CSaveLoadMgr::Load(string fileName)
{
}

void CSaveLoadMgr::Free()
{
    // TODO: vector 내부에 있는 map data들 전부 싹 날려주기
}
