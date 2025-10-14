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

        addPointLight();
        
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
        OnMaterialModified();

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
            AssignMaterial(entity.GetUUID(), defaultMaterialID, i);
            meshFilterComp.names[i] = meshResource->getSubMesh(i)->name;
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
            AssignMaterial(entity.GetUUID(), defaultMaterialID, i);
            meshFilterComp.names[i] = meshResource->getSubMesh(i)->name;
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
    }

    Entity& Scene::addCube(const std::string& name)
    {
        auto meshResource = cache->getByName<Mesh>("Cube");
        UUID defaultMtlID = cache->getDefaultMaterial()->GetID();

        Entity& entity = createEntity(name);
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(meshResource->GetID());

        meshFilterComp.names.resize(1);

        AssignMaterial(entity.GetUUID(), defaultMtlID, 0);
        meshFilterComp.names[0] = "Sub" + meshResource->getSubMesh(0)->name;

        return entity;
    }

    Entity& Scene::addPlane(const std::string& name)
    {
        auto meshResource = cache->getByName<Mesh>("Plane");
        UUID defaultMtlID = cache->getDefaultMaterial()->GetID();;

        Entity& entity = createEntity(name);
        auto& meshFilterComp = entity.AddComponent<MeshFilterComponent>(cache->getByName<Mesh>("Plane")->GetID());
        meshFilterComp.names.resize(1);

        AssignMaterial(entity.GetUUID(), defaultMtlID, 0);
        meshFilterComp.names[0] = "Sub" + meshResource->getSubMesh(0)->name;
        return entity;
    }

    void Scene::AssignMaterial(UUID entityID, UUID mtlID, int subMeshIndex)
    {
        auto& entt = GetEntity(entityID);
        UUID previousMTLID = entt.GetSubMeshMaterial(subMeshIndex);
        cache->unsee<Material>(previousMTLID);
        entt.AssignMaterial(mtlID, subMeshIndex);
        cache->use<Material>(mtlID);
    }

    void Scene::ResetMaterial(UUID entityID, int subMeshIndex)
    {
        auto& entt = GetEntity(entityID);

        UUID previousMTLID = entt.GetSubMeshMaterial(subMeshIndex);
        cache->unsee<Material>(previousMTLID);
        entt.ResetMaterial(subMeshIndex);
        cache->use<Material>(cache->getDefaultMaterial()->GetID());
    }

    void Scene::AssigGlobalMaterial(UUID entityID, UUID mtlID)
    {
        auto& entt = GetEntity(entityID);
        
        auto materials = entt.GetMaterialsInUse();
        for (auto& mtl : materials)
        {
            cache->unsee<Material>(mtl);
        }      

        entt.AssignGlobalMaterial(mtlID);
        cache->use<Material>(cache->getDefaultMaterial()->GetID(), entt.GetSubMeshCount());
    }

    void Scene::ResetAllMaterial(UUID entityID)
    {
        auto& entt = GetEntity(entityID);

        auto materials = entt.GetMaterialsInUse();
        for (auto& mtl : materials)
        {
            cache->unsee<Material>(mtl);
        }

        entt.ResetAllSubMeshMaterials();
        cache->use<Material>(cache->getDefaultMaterial()->GetID(), entt.GetSubMeshCount());
    }


    void Scene::removeEntity(Entity entity)
    {
        removeFromVector<Entity>(entities, entity);
        registry.destroy(entity.GetBaseEntity());

        OnEntityRegisteryModified();
        OnMaterialModified();
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
            {"Resource",fkyaml::node::sequence()}
        };

        std::vector<fkyaml::node> enttNodes;
        for (auto entt : entities)
        {
            auto idString = UUIDFactory::uuidToString(entt.GetUUID());
            auto tag = entt.GetTag();
            auto active = entt.GetActive();
            auto transform = entt.GetComponent<TransformComponent>();

            fkyaml::node temp = {
                {"UUID",idString},
                {"Tag", tag},
                {"Active", active},
                {
                    "Transform",
                    {
                        {transform.position.x,transform.position.y,transform.position.z},
                        {transform.rotation.x,transform.rotation.y,transform.rotation.z},
                        {transform.scale.x,transform.scale.y,transform.scale.z}
                    }
                }
            };

            if (entt.HasComponent<MeshFilterComponent>())
            {
                auto meshFilter = entt.GetComponent<MeshFilterComponent>();
                fkyaml::node meshFilterNode = {
                    {"UUID", UUIDFactory::uuidToString(meshFilter.meshID)},
                };

                fkyaml::ordered_map<std::string, std::vector<int>> materialMapTemp;
                for (auto& [mtl, dependants] : meshFilter.materialIDMap)
                {
                    for (auto dep : dependants)
                        materialMapTemp[UUIDFactory::uuidToString(mtl)].push_back(dep);
                }
                meshFilterNode["Material"] = materialMapTemp;

                std::vector<std::string> namesNode(entt.GetSubMeshCount());
                int i = 0;
                for (auto& name : meshFilter.names)
                {
                    namesNode[i++] = name;
                }
                meshFilterNode["Names"] = namesNode;

                temp["MeshFilter"] = meshFilterNode;
            }

            if (entt.HasComponent<LightComponent>())
            {
                auto& lightComp = entt.GetComponent<LightComponent>();
                fkyaml::node lightNode = {
                    {"Type", GetLightTypeString(lightComp.type)},
                    {"Color", {lightComp.color.x,lightComp.color.y,lightComp.color.z,lightComp.color.w} },
                    {"Position", {lightComp.position.x,lightComp.position.y,lightComp.position.z} },
                    {"Inner Radius", lightComp.innerRadius},
                    {"Outer Radius", lightComp.outerRadius},
                    {"Spot Direction", {lightComp.spotDirection.x, lightComp.spotDirection.y, lightComp.spotDirection.z}}
                };

                temp["Light"] = lightNode;
            }
            
            enttNodes.emplace_back(temp);
        }
        node["Entities"] = enttNodes;

        std::vector<fkyaml::node> resourceNodes;
        for (auto& resource : cache->getByType<ImageTexture>(ResourceType::ImageTexture))
        {
            fkyaml::node resourceNode = {
                {"Path",resource->getPath().string()},
                {"Type",(int)resource->getType()},
                {"UUID",UUIDFactory::uuidToString(resource->GetID())}
            };
            resourceNodes.push_back(resourceNode);
        }

        for (auto& resource : cache->getByType<Mesh>(ResourceType::Mesh))
        {
            fkyaml::node resourceNode = {
                {"Path",resource->getPath().string()},
                {"Type",(int)resource->getType()},
                {"UUID",UUIDFactory::uuidToString(resource->GetID())}
            };
            resourceNodes.push_back(resourceNode);
        }
        node["Resource"] = resourceNodes;


        std::ofstream file(path);
        file << node;

        file.close();
    }

}
