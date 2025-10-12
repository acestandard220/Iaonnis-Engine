#include "Scene.h"
#include "Entity.h"


namespace Iaonnis
{
    Entity* g = nullptr;
    Iaonnis::Scene::Scene(const std::string& name)
        :cache(cache),name(name), displaySize(glm::vec2(800.0f, 800.0f))
    {
        cache = std::make_shared<ResourceCache>();
        camera = std::make_shared<Camera>("Main Camera", glm::vec3(3.0f, 3.0f, 8.0f), displaySize.x, displaySize.y);
        environment = cache->load<Environment>("Assets/Environment Maps/Skybox/skybox.txt");

        auto cubeEntity = addCube("Cube1");
        //addMesh("Assets/obj_sphere.obj", "Sphere");
        //addDirectionalLight(glm::vec3(1.0f, 2.0f, 1.0f));
        addPointLight();

        //auto grayRockDiff = cache->load<ImageTexture>("Assets\\Textures\\Gray Rocks\\GrayRocksDiff_2k.png");
        //auto grayRockNorm = cache->load<ImageTexture>("Assets\\Textures\\Gray Rocks\\GrayRocksNorGl_2k.png");
        //auto grayRockAo   = cache->load<ImageTexture>("Assets\\Textures\\Gray Rocks\\GrayRocksAO_2K.png");
        //auto grayRockRough = cache->load<ImageTexture>("Assets/Textures/Gray Rocks/GrayRocksRough_2k.png");
        //auto grayRockMetallic = cache->load<ImageTexture>("Assets/Textures/default_metallic.png");
        //
        //auto grayRockMtl = cache->create<Material>("Gray Rock Material.yaml");
        //grayRockMtl->setDiffuseMap(grayRockDiff->GetID());
        //grayRockMtl->setNormalMap(grayRockNorm->GetID());
        //grayRockMtl->setAoMap(grayRockAo->GetID());
        //grayRockMtl->setRoughnessMap(grayRockRough->GetID());
        //grayRockMtl->setMetallicMap(grayRockMetallic->GetID());

        auto rustyMetalDiff = cache->load<ImageTexture>("Assets/Textures/Rusty Metal_4k/diff.png");
        auto rustyMetalNorm = cache->load<ImageTexture>("Assets/Textures/Rusty Metal_4k/normal.png");
        auto rustyMetalAo = cache->load<ImageTexture>("Assets/Textures/Rusty Metal_4k/ao.png");
        auto rustyMetalRough = cache->load<ImageTexture>("Assets/Textures/Rusty Metal_4k/roughness.png");
        auto rustyMetalMetallic = cache->load<ImageTexture>("Assets/Textures/Rusty Metal_4k/metallic.png");
        
        auto rustyMetalMtl = cache->create<Material>("Rusty Metal.yaml");
        rustyMetalMtl;
        rustyMetalMtl->setDiffuseMap(rustyMetalDiff->GetID());
        rustyMetalMtl->setNormalMap(rustyMetalNorm->GetID());
        rustyMetalMtl->setAoMap(rustyMetalAo->GetID());
        rustyMetalMtl->setRoughnessMap(rustyMetalRough->GetID());
        rustyMetalMtl->setMetallicMap(rustyMetalMetallic->GetID());

        cubeEntity.AssignMaterial(rustyMetalMtl->GetID(), 0);
        

        //addMesh("Assets/Models/Lion_Head_4k/LionHead.obj","BackPack");
        addMesh("Assets/backpack.obj","BackPack");

        //Systems Init()
        systems.emplace_back(std::make_unique<TransformSystem>(&registry));



        EventBus::subscribe(EventType::RESIZE_EVENT, std::bind(&Scene::OnViewFrameResize, this, std::placeholders::_1));
    }

    Scene::~Scene()
    {

    }

    void Scene::OnUpdate(float dt)
    {
        for (auto& system : systems)
            system->OnUpdate(dt);
    }

    Entity& Iaonnis::Scene::createEntity(const std::string& name)
    {
        Entity entity{ registry.create(),this};

        entity.AddComponent<IDComponent>();
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();

        entities.push_back(entity);

        OnEntityRegisteryModified();
        return entities.back();
    }

    Entity& Scene::CreateCamera(const std::string& name)
    {
        Entity& entity = createEntity(name);
        auto camera = std::make_shared<Camera>(name, glm::vec3(0.0f, -2.0f, -5.0f), displaySize.x, displaySize.y);
        auto& camComp = entity.AddComponent<CameraComponent>();
        camComp.camera = camera;
        camComp.primary = true;

        return entity;
    }

    Entity& Scene::addMesh(filespace::filepath path, const std::string& name)
    {
        std::shared_ptr<Mesh> meshResource = cache->load<Mesh>(path);
        int subMeshCount = meshResource->getSubMeshCount();
        UUID defaultMaterialID = cache->getDefaultMaterial()->GetID();

        Entity& entity = createEntity(meshResource->getName());
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(meshResource->GetID());
        meshFilterComp.names.resize(subMeshCount);
        
        for (int i = 0; i < subMeshCount; i++)
        {
            entity.AssignMaterial(defaultMaterialID, i);
            meshFilterComp.names[i] = meshResource->getSubMesh(i)->name;
            cache->use<Material>(defaultMaterialID);
        }



        return entity;
    }

    Entity& Scene::addMesh(UUID meshID)
    {
        std::shared_ptr<Mesh> meshResource = cache->getByUUID<Mesh>(meshID);
        int subMeshCount = meshResource->getSubMeshCount();
        UUID defaultMaterialID = cache->getDefaultMaterial()->GetID();

        Entity& entity = createEntity(meshResource->getName());
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(meshResource->GetID());
        meshFilterComp.names.resize(subMeshCount);

        for (int i = 0; i < subMeshCount; i++)
        {
            entity.AssignMaterial(defaultMaterialID, i);
            meshFilterComp.names[i] = meshResource->getSubMesh(i)->name;
            cache->use<Material>(defaultMaterialID);
        }
        return entity;
    }

    Entity& Iaonnis::Scene::addDirectionalLight(glm::vec3 direction )
    {
        Entity& entity = createEntity("Directional Light");
        auto& lightComp = entity.AddComponent<LightComponent>();
        lightComp.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        lightComp.type = LightType::Directional;
        lightComp.position = direction;

        return entity;
    }

    Entity& Scene::addSpotLight()
    {
        Entity& entity = createEntity("Spot Light");
        auto& lightComp = entity.AddComponent<LightComponent>();
        lightComp.color = glm::vec4(0.8f, 0.6f, 0.6f, 1.0f);
        lightComp.type = LightType::Spot;
        lightComp.position = glm::vec3(0.0f, 3.0f, 0.0f);

        lightComp.innerRadius = 5.0f;
        lightComp.outerRadius = 30.0f;
        lightComp.spotDirection = glm::vec3(0.0f, -1.0f, 0.0f);

        return entity;
    }

    Entity& Scene::addPointLight()
    {
        Entity& entity = createEntity("Point Light");
        auto& lightComp = entity.AddComponent<LightComponent>();
        lightComp.color = glm::vec4(0.8f, 0.6f, 0.6f, 1.0f);
        lightComp.type = LightType::Point;
        lightComp.position = glm::vec3(0.0f, 1.5f, 1.5f);

        return entity;
    }

    Entity& Scene::addCamera()
    {
        Entity& entity = createEntity("Camera");
        auto& cameraComp = entity.GetComponent<CameraComponent>();
        cameraComp;

        return entity;
        // TODO: insert return statement here
    }

    Entity& Scene::addCube(const std::string& name)
    {
        auto meshResource = cache->getByName<Mesh>("Cube");
        UUID defaultMtlID = cache->getDefaultMaterial()->GetID();

        Entity& entity = createEntity(name);
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(meshResource->GetID());

        meshFilterComp.names.resize(1);

        entity.AssignMaterial(defaultMtlID, 0);
        meshFilterComp.names[0] = "Sub" + meshResource->getSubMesh(0)->name;

        cache->use<Material>(defaultMtlID);
        return entity;
    }

    Entity& Scene::addPlane(const std::string& name)
    {
        auto meshResource = cache->getByName<Mesh>("Plane");
        UUID defaultMtlID = cache->getDefaultMaterial()->GetID();;

        Entity& entity = createEntity(name);
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(cache->getByName<Mesh>("Plane")->GetID());
        meshFilterComp.names.resize(1);

        entity.AssignMaterial(defaultMtlID, 0);
        meshFilterComp.names[0] = "Sub" + meshResource->getSubMesh(0)->name;

        cache->use<Material>(defaultMtlID);
        return entity;
    }

    //Entity has a fast helper function. Use that instead.
    void Scene::AssignMaterial(UUID entityID, UUID mtlID, int subMeshIndex)
    {
        Entity* entity = nullptr;
        for (auto entt : entities)
        {
            if (entt.GetUUID() == entityID)
            {
                entity = &entt;
                break;
            }
        }

        if (entity == nullptr)
        {
            IAONNIS_LOG_ERROR("Failed to find entity. (UUID = %s)", UUIDFactory::uuidToString(entityID));
            return;
        }

        auto meshFilter = entity->GetComponent<MeshFilterComponent>();
        auto& materialMap = meshFilter.materialIDMap;

        for (auto& [matID, dependants] : materialMap)
            dependants.remove(subMeshIndex);

        meshFilter.materialIDMap[mtlID].push_back(subMeshIndex);
        return;
    }

    void Scene::removeEntity(Entity entity)
    {
        //removeFromVector<Entity>(entities, entity);
        //registry.remove<entt::entity>(entity.GetBaseEntity());
    }

    Entity& Scene::GetEntity(UUID id)
    {
        for (auto& entt : entities)
        {
            if (entt.GetUUID() == id)
                return entt;
        }
    }

    void Scene::OnViewFrameResize(Event& event)
    {
        FrameResizeEvent* frameResizeEvent = (FrameResizeEvent*)&event;
        camera->setAspectRatio(frameResizeEvent->frameSizeX, frameResizeEvent->frameSizeY);
    }

    void Scene::save(filespace::filepath path)
    {
        fkyaml::node node{
            {"Name",name},
            {"No. Entt",entities.size()},
            {"Entities",fkyaml::node::sequence()},
            {"Resource",fkyaml::node::sequence()}
        };

        auto& entts = node["Entities"].as_seq();

        //Note Attempting to write out Mesh filter comp even if it doesn't have one.
        //It breaks the program during save.
        for (auto entt : entities)
        {
            auto idString = UUIDFactory::uuidToString(entt.GetUUID());
            auto tag = entt.GetTag();
            auto active = entt.GetActive();
            auto transform = entt.GetComponent<TransformComponent>();
            auto meshFilter = entt.GetComponent<MeshFilterComponent>();

            fkyaml::node temp = {
                {"UUID",idString},
                {"Tag", tag},
                {"Active", active},
                {
                    "Transform",
                    {
                        "Position",{transform.position.x,transform.position.y,transform.position.z},
                        "Rotation",{transform.rotation.x,transform.rotation.y,transform.rotation.z},
                        "Scale",{transform.scale.x,transform.scale.y,transform.scale.z}
                    }
                },
                {
                    "MeshFilter",
                    {
                        {"UUID", UUIDFactory::uuidToString(meshFilter.meshID)},
                        {"Materials",fkyaml::node::sequence()},
                        {"Names",fkyaml::node::sequence()}
                    }
                }
            };

            /*if(entt.HasComponent<MeshFilterComponent>())
            {
                auto& mtlIDs = temp["MeshFilter"].as_map().at("Materials").as_seq();
                for (auto mtlID : meshFilter.materialID)
                {
                    mtlIDs.push_back(UUIDFactory::uuidToString(mtlID));
                }

                auto& names = temp["MeshFilter"].as_map().at("Names").as_seq();
                for (auto name : meshFilter.names)
                {
                    mtlIDs.push_back(name);
                }
 
            }*/


            
            entts.emplace_back(temp);
        }



        std::ofstream file(path);
        file << node;
    }

}
