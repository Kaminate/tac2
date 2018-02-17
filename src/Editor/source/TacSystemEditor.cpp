// Nathan Park
#include "TacSystemEditor.h"
#include "TacSystemGraphics.h"
#include "TacCore.h"
#include "TacErrorIf.h"
#include "TacComponentTransform.h"
#include "TacComponentModel.h"
#include "TacComponentCamera.h"
#include "TacCamera.h"
#include "TacRaycast.h"
#include "TacGeometryData.h"
#include "TacErrorIf.h"
#include "TacMathUtil.h"
#include "TacMatrix3.h"

#include <iomanip>

namespace Tac
{
  EditorSystem::EditorSystem()
    : mIDToSelect(Entity::InvalidId)
    , mInputMsgHandler(new InputMsgHandler(InputMsgHandler::eEditor, 
    "Editor Input Msg Handler",
    [this](const InputMsg & msg){return OnMsg(msg);}))
    , mCameraMovementSpeed(1)
    , mMaxDegPerSec(135.0f)
    , mCamInvertedHorizControls(true)
    , mCamInvertedVertiControls(true)
  {}

  void EditorSystem::Update( float dt )
  {

    GraphicsSystem * graphics;
    GET_SYSTEM(graphics, GetCore(), System::eSystemGraphics);

    CameraComponent * camComp = graphics->GetCurrentCamera();
    if(!camComp)
      return;
    Camera & cam = camComp->mCamera;



    // todo: make the input gathering from update ignore it if
    // control modifier keys are down 
    // (ie: ctrl+s shouldn't move down while in selection mode )

    InputSystem * input;
    GET_SYSTEM(input, GetCore(), System::eSystemInput);

    auto EitherKey = [input](int key1, int key2){
      return input->IsKeyDown(key1) || input->IsKeyDown(key2);
    };

    bool controlDown 
      = EitherKey(GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL);
    bool navL = EitherKey(GLFW_KEY_LEFT, GLFW_KEY_A) && !controlDown;
    bool navR = EitherKey(GLFW_KEY_RIGHT, GLFW_KEY_D) && !controlDown;
    bool navU = EitherKey(GLFW_KEY_UP, GLFW_KEY_W) && !controlDown;
    bool navD = EitherKey(GLFW_KEY_DOWN, GLFW_KEY_S) && !controlDown;



    Vector3 addVec(0,0,0);

    TransformComp * selectedTrans = nullptr;
    ModelComponent * selectedModel = nullptr;
    if( mSelectedEntity )
    {
      Entity * selected = mSelectedEntity->GetSelected();
      if(selected->HasComponent(Comp::eCompTransform))
        GET_COMP(selectedTrans, selected, Comp::eCompTransform);
      if(selected->HasComponent(Comp::eCompModel))
        GET_COMP(selectedModel, selected, Comp::eCompModel);
    }
    if( selectedTrans )
    {
      Vector3 objectOrbitCenter = selectedTrans->mPos;
      if(selectedModel)
      {
        const BoundingSphere & selectedBoundingSphere
          = selectedModel->GetGeometry()->GetBoundingSphere();
        objectOrbitCenter += 
          (selectedTrans->mFakeTransform 
          * Vector4(selectedBoundingSphere.mPos, 0)).XYZ();
      }

      // orbit the entity (spherical coordinates relative to the entity)
      Vector3 originalToCam = cam.mPosition - objectOrbitCenter;
      Vector3 toCam = cam.mPosition - objectOrbitCenter;

      float angleRadians = ToRadians(dt * mMaxDegPerSec);
      Vector2 angleToRotate(
        ToRadians(dt * mCamDegPerSec.x),
        ToRadians(dt * mCamDegPerSec.y));

      Vector3 right = cam.mWorldSpaceUp.Cross( toCam );
      if( right.LengthSq() > EPSILON )
        right.Normalize();

      float degPosToUp = ToDegrees( 
        acos( toCam.Dot( cam.mWorldSpaceUp ) / toCam.Length() ) );

      if( navU ^ navD ) // <-- only for damping hack
      {
        mCamDegPerSec.y = mMaxDegPerSec * (navD - navU);
      }

      const float degBuf = 5;

      if( mCamDegPerSec.y < 0 && degPosToUp < degBuf) // rotating up
        mCamDegPerSec.y = 0;

      if( mCamDegPerSec.y > 0 && degPosToUp > 180 - degBuf) 
        mCamDegPerSec.y = 0;

      toCam = Matrix3::VectorAlignment( right, 
        ToRadians(dt * mCamDegPerSec.y) )
        * toCam;

      if(navL) mCamDegPerSec.x = -mMaxDegPerSec;
      if(navR) mCamDegPerSec.x = mMaxDegPerSec;
      toCam = Matrix3::RotRadZ(ToRadians(dt * mCamDegPerSec.x)) * toCam;

      mCamDegPerSec *= 0.80f; // damping

      // DAMP the camera view direction towards the selected object
      Vector3 desiredViewDir = -toCam;
      desiredViewDir.Normalize();
      Vector3 normalizedOrigToCam = originalToCam;
      normalizedOrigToCam.Normalize();
      float damping = mCamDegPerSec.LengthSq() > EPSILON ? 1.0f : 0.25f;
      Vector3 toDesired = desiredViewDir - cam.mViewDirection;
      cam.mViewDirection += toDesired * damping;
      cam.mViewDirection.Normalize();

      // SNAP the camera position
      addVec += toCam - originalToCam;

      // in selected mode, zoom in/out with q/e
      if( input->IsKeyDown(GLFW_KEY_Q)) 
      {
        bool canZoomIn = true;
        if(selectedModel)
        {
          const Vector3 & s = selectedTrans->mSca;
          const BoundingSphere & boundSphere 
            = selectedModel->GetGeometry()->GetBoundingSphere();
          float maxScale = std::max(std::max(s.x, s.y), s.z);
          float minDist 
            = 0.1f 
            + maxScale * boundSphere.mRadius
            + cam.mNear;

          if( originalToCam.Length() < minDist )
          {
            canZoomIn = false;
          }
        }
        addVec += desiredViewDir * mCameraMovementSpeed;
      }
      if( input->IsKeyDown(GLFW_KEY_E)) 
        addVec -= desiredViewDir * mCameraMovementSpeed;
    }
    else
    {
      Vector3 right(0,0,0);
      Vector3 up(0,0,0);
      cam.GetCamDirections(right, up);
      if( !input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) 
        && !input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
      {
        Matrix3 rotX = Matrix3::VectorAlignment( 
          up, ToRadians(mCamNonSelDegPerSec.x / 60.0f));
        Matrix3 rotY = Matrix3::VectorAlignment( 
          right, ToRadians(mCamNonSelDegPerSec.y / 60.0f));
        mCamNonSelDegPerSec *= 0.80f; // damping
        cam.mViewDirection = rotX * rotY * cam.mViewDirection;
        cam.mViewDirection.Normalize();
      }


      Vector3 movementDir(0,0,0);
      if( navU ) movementDir += cam.mViewDirection;
      if( navL ) movementDir -= right;
      if( navD ) movementDir -= cam.mViewDirection;
      if( navR ) movementDir += right;

      // clamp to xy plane
      movementDir.z = 0;
      if(movementDir.LengthSq() > EPSILON)
        movementDir.Normalize();

      if( input->IsKeyDown(GLFW_KEY_Q)) movementDir -= Vector3(0,0,1);
      if( input->IsKeyDown(GLFW_KEY_E)) movementDir += Vector3(0,0,1);

      addVec += movementDir * mCameraMovementSpeed;

    }

    TransformComp * cameraTransform;
    GET_SIBL(cameraTransform, camComp, Comp::eCompTransform );

    // uhh, update all the things
    cam.mPosition += addVec;
    cameraTransform->mPos += addVec;
    cameraTransform->mFakePos += addVec;
  }

  void EditorSystem::UpdateFakeTime( float dt )
  {

  }

  void EditorSystem::OpenLevel()
  {
    std::string filename;
    std::vector<Window::FileTypeThing> things;
    things.push_back(Window::FileTypeThing(
      "level (.taclevel)", "taclevel"));
    if(GetCore()->GetWindow().OpenFile(filename, things))
    {
      OpenLevel(filename);
    }
  }
  void EditorSystem::SaveLevel()
  {
    const std::string & path = GetCurrentLevelPath();
    if (path.empty())
    {
      SaveLevelAs();
    }
    else
    {
      GetCore()->Save(path);
    }
  }
  void EditorSystem::SaveLevelAs()
  {
    std::string filename;
    std::vector<Window::FileTypeThing> things;
    things.push_back(Window::FileTypeThing(
      "level (.taclevel)", "taclevel"));
    things.push_back(Window::FileTypeThing(
      "text (.txt)", "txt"));
    if(GetCore()->GetWindow().SaveFile(filename, things))
    {
      SetCurrentLevelPath(filename);
      GetCore()->Save(filename);
    }
  }

#ifdef TACTWEAK
  void TW_CALL EditorSystem::CallbackOpenLevel(void * data)
  {
    EditorSystem * editor = (EditorSystem*) data;
    editor->OpenLevel();
  }
  void TW_CALL EditorSystem::CallbackSaveLevelAs(void * data)
  {
    EditorSystem * editor = (EditorSystem*) data;
    editor->SaveLevelAs();
  }
  void TW_CALL EditorSystem::CallbackSaveLevel(void * data)
  {
    EditorSystem * editor = (EditorSystem*) data;
    editor->SaveLevel();
  }
  void TW_CALL EditorSystem::CallbackCreateEntity(void * data)
  {
    EditorSystem * editor = (EditorSystem *) data;
    Entity * gameObj = editor->GetCore()->CreateEntity();
    editor->AddEntityToList(gameObj);
    editor->SelectEntity(gameObj);
  }
  void TW_CALL EditorSystem::CallbackOnSelectEntity(void * data)
  {
    EditorSystem * editor = (EditorSystem *) data;
    Core * engine = editor->GetCore();
    Entity * gameObj = engine->GetEntity(editor->GetIdToSelect());
    if (gameObj)
      editor->SelectEntity(gameObj);
  }
#endif

  void EditorSystem::SetCurrentLevelPath( const std::string & levelPath )
  {
    mLevel = levelPath;
  }

  const std::string & EditorSystem::GetCurrentLevelPath()
  {
    return mLevel;
  }

  void EditorSystem::SelectEntity( Entity *gameObj )
  {

    if (gameObj)
    {
      bool alreadySelected 
        = mSelectedEntity 
        && mSelectedEntity->GetSelected() == gameObj;

      if (!alreadySelected)
      {
        mSelectedEntity = SPSelectedEntity(
          new SelectedEntity(this, gameObj));

      }
    }
    else
    {
      mSelectedEntity.reset();
    }
  }

  void EditorSystem::AddEntityToList( Entity *gameObj )
  {
#ifdef TACTWEAK

    std::string name = std::to_string(gameObj->GetID());
    mTweakMenu->AddComment(name, name);
#else
    UnusedParameter(gameObj);
#endif

  }

  void EditorSystem::RemEntityFromList( Entity* gameObj )
  {
#ifdef TACTWEAK

    std::string name = std::to_string(gameObj->GetID());
    mTweakMenu->RemoveVar(name);
#else
    UnusedParameter(gameObj);
#endif
  }

  Entity::ID EditorSystem::GetIdToSelect()
  {
    return mIDToSelect;
  }

  void EditorSystem::CLearList()
  {
    mSelectedEntity.reset();
    for (auto pair : GetCore()->GetEntities())
    {
      Entity * gameObj = pair.second;
      RemEntityFromList(gameObj);
    }
  }

  void EditorSystem::OpenLevel( const std::string & filename )
  {
    Core * engine = GetCore();
    // order of these calls is semi important
    CLearList();
    engine->ClearEntities();
    engine->Load(filename);
    SetCurrentLevelPath(filename);
    for (auto pair: engine->GetEntities())
    {
      Entity * entity = pair.second;
      AddEntityToList(entity);
    }

    // set current camera, light
    GraphicsSystem * graphics
      =  (GraphicsSystem*) engine->GetSystem(System::eSystemGraphics);

    graphics->SetCurrentCamera(
      (CameraComponent*)graphics->GetRandomComp(
      Comp::Type::eCompCamera).get());

    graphics->SetCurrentLight(
      (LightComponent*)graphics->GetRandomComp(
      Comp::Type::eCompLight).get());
  }

  static Vector3 GetMouseViewSpace(
    int xScreenSpace, 
    int yScreenSpace,
    int windowW,
    int windowH,
    const Camera & cam)
  {
    float xNDC = 2.0f * xScreenSpace / windowW - 1;
    float yNDC = -2.0f * yScreenSpace / windowH + 1;


    float theta = cam.mFieldOfViewYRad / 2.0f;
    float cotTheta = 1.0f / tan(theta);
    float sX  =  cotTheta / cam.mAspectRatio;
    float sY = cotTheta;

    float xVS = cam.mNear * xNDC / sX;
    float yVS = cam.mNear * yNDC / sY;

    return Vector3(xVS, yVS, -cam.mNear);
  }

  bool EditorSystem::OnMsg( const InputMsg & msg )
  {
    InputSystem * input;
    GET_SYSTEM(input, GetCore(), System::eSystemInput);

    Core * engine = GetCore();
    GraphicsSystem * graphics;
    GET_SYSTEM(graphics, engine, System::eSystemGraphics);

    CameraComponent * curCamera = graphics->GetCurrentCamera();
    if (!curCamera)
      return false;

    Camera & cam = curCamera->mCamera;
    TransformComp * cameraTrans;
    GET_SIBL(cameraTrans, curCamera, Comp::eCompTransform);


    bool msgHandled = false;

    switch (msg.mType)
    {
    case InputMsg::eMouseClick:
      {
        if( msg.mMouseClick.mAction == GLFW_PRESS 
          && msg.mMouseClick.mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
          mMouseClickPos.x = (float)input->GetMouseX();
          mMouseClickPos.y = (float)input->GetMouseY();
        }

        bool canSelect = true;
        canSelect &= msg.mMouseClick.mAction == GLFW_RELEASE;
        canSelect &= msg.mMouseClick.mButton == GLFW_MOUSE_BUTTON_LEFT;
        if( mSelectedEntity)
        {
          bool mouseMoved 
            = input->GetMouseX() - mMouseClickPos.x != 0 
            && input->GetMouseY() - mMouseClickPos.y != 0;

          if( mouseMoved)
          {
            canSelect = false;
          }
        }

        if(canSelect)
        {
          // todo: use mouse pos to get rayDir
          int mouseX = input->GetMouseX();
          int mouseY = input->GetMouseY();

          const Window & wind = GetCore()->GetWindow();
          Vector3 mouseViewSpace = GetMouseViewSpace(
            mouseX, 
            mouseY, 
            wind.GetWidth(), 
            wind.GetHeight(),
            curCamera->mCamera);

          Vector3 mouseWorldSpace 
            = (curCamera->mCamera.GetInverseViewMatrix() 
            * Vector4(mouseViewSpace, 1)).XYZ();

          //Vector3 rayDir = curCamera->mCamera.mViewDirection;
          Vector3 rayOrigin = cameraTrans->mFakePos;
          Vector3 rayDir = mouseWorldSpace - rayOrigin;
          rayDir.Normalize();

          Entity * closestEntity = nullptr;
          float closestDist = FLT_MAX;

          const Core::EntityContainer & entities = engine->GetEntities();
          for (const auto & entityPair : entities)
          {
            Entity * gameObject = entityPair.second;

            if(!gameObject->HasComponent(Comp::eCompModel))
              continue;
            if(!gameObject->HasComponent(Comp::eCompTransform))
              continue;

            ModelComponent * model;
            GET_COMP(model, gameObject, Comp::eCompModel);

            TransformComp * transform;
            GET_COMP(transform, gameObject, Comp::eCompTransform);
            const Matrix4 & world = transform->mFakeTransform;

            float boundingSphereScale = std::max(
              transform->mSca.x, std::max(
              transform->mSca.y, 
              transform->mSca.z));

            const Geometry * objspaceGeom = model->GetGeometry();

            const BoundingSphere & objectSpaceSphere 
              = objspaceGeom->GetBoundingSphere();

            float dist;
            // before raycast sphere, test if the ray is inside the sphere
            // if (!insideSphere(rayOrigin, sphere)
            // RaycastSphere(objectSpaceSphere.mPos + transform->mPos, 
            // objectSpaceSphere.mRadius * boundingSphereScale,
            // cameraTrans->mPos,
            // curCamera->mCamera.mViewDirection, 
            // dist);

            const GeometryData & data = objspaceGeom->GetGeometryData();
            int numTris = data.mCounts[Attribute::ePos] / 3;
            for (int triIndex = 0; triIndex < numTris; ++triIndex)
            {
              int baseIndex = triIndex * 3;

              // model space points
              const Vector3 & p0MS = data.mPos[baseIndex + 0];
              const Vector3 & p1MS = data.mPos[baseIndex + 1];
              const Vector3 & p2MS = data.mPos[baseIndex + 2];

              // world space points
              Vector3 p0 = (world * Vector4(p0MS, 1)).XYZ();
              Vector3 p1 = (world * Vector4(p1MS, 1)).XYZ();
              Vector3 p2 = (world * Vector4(p2MS, 1)).XYZ();

              // todo: dot product of ray direction with triangle normal
              bool hit = RaycastTriangle(
                p0,
                p1, 
                p2,
                rayOrigin,
                rayDir,
                dist);

              if (!hit)
                continue;

              if (dist < closestDist)
              {
                closestEntity = gameObject;
                closestDist = dist;
              }
            } // for each tri
          } // for each entity

          SelectEntity(closestEntity);
          if(closestEntity) msgHandled = true;
        } // if (msg.mouseclick.action == press)

        break;
      }
    case InputMsg::eMouseScroll:
      {
        double dy = msg.mMouseScroll.mOffsetY;

        cameraTrans->mPos 
          += cam.mViewDirection 
          * ( float ) dy 
          * mCameraMovementSpeed;
      }
      break;


    case InputMsg::eMouseMove:
      {
        if( input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT) ||
          input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) )
        {
          int dX = input->GetMouseX() - input->GetLastMouseX();
          int dY = input->GetMouseY() - input->GetLastMouseY();
          if( mCamInvertedHorizControls ) dX *= -1;
          if( mCamInvertedVertiControls ) dY *= -1;
          float dt = 1/60.0f;
          float degPer100PxSELECTED = 60.0f;
          float degPer100PxFREE = 10.0f;
          float degPerPxSELECTED = degPer100PxSELECTED / 100;
          float degPerPxFREE = degPer100PxFREE / 100;

          float degXdtSELECTED = degPerPxSELECTED * dX;
          float degXdtFREE = degPerPxFREE * dX;
          float currAngularVelXDegSELECTED = degXdtSELECTED / dt;
          float currAngularVelXDegFREE = degXdtFREE / dt;

          float degYdtSELECTED = degPerPxSELECTED * dY;
          float degYdtFREE = degPerPxFREE * dY;
          float currAngularVelYDegSELECTED = degYdtSELECTED / dt;
          float currAngularVelYDegFREE = degYdtFREE / dt;


          if( mSelectedEntity) // orbit selected object
          {
            mCamDegPerSec.x = currAngularVelXDegSELECTED;
            mCamDegPerSec.y = currAngularVelYDegSELECTED;
          }
          else // turn camera
          {
            mCamNonSelDegPerSec.x = currAngularVelXDegFREE;
            mCamNonSelDegPerSec.y = currAngularVelYDegFREE;
            Vector3 right(0,0,0); 
            Vector3 up(0,0,0);
            cam.GetCamDirections(right, up);
            Matrix3 rotX = Matrix3::VectorAlignment( up, ToRadians(degXdtFREE));
            Matrix3 rotY = Matrix3::VectorAlignment( right, ToRadians(degYdtFREE));

            cam.mViewDirection = rotX * rotY * cam.mViewDirection;
            cam.mViewDirection.Normalize();
          }
        }
      }
    case InputMsg::eKey:
      if(msg.mKey.action != GLFW_PRESS)
        break;

      switch (msg.mKey.key)
      {
      case GLFW_KEY_DELETE: // fallthrough
      case GLFW_KEY_BACKSPACE:
        DeleteSelected();
        msgHandled = true;
        break;
      case GLFW_KEY_O:
        if( msg.mKey.mods & GLFW_MOD_CONTROL )
        {
          OpenLevel();
          msgHandled = true;
        }
        break;
      case GLFW_KEY_S:
        if( msg.mKey.mods & GLFW_MOD_CONTROL )
        {
          SaveLevel();
          msgHandled = true;
        }
        break;
      default:
        break;
      }

    default:
      break;
    } // switch(inputMsg.mType)
    return msgHandled;
  } // EditorSystem::OnMsg()

  EditorSystem::~EditorSystem()
  {
    InputSystem * input;
    GET_SYSTEM(input, GetCore(), System::eSystemInput);
    input->RemHandler(mInputMsgHandler);
  }

  void EditorSystem::DeleteSelected()
  {
    if(!mSelectedEntity)
      return;

    Entity * gameObj = mSelectedEntity->GetSelected();
    GetCore()->DeleteEntity( gameObj->GetID() );

    mSelectedEntity.reset();
  }

  void EditorSystem::Init()
  {
    InputSystem * input;
    GET_SYSTEM(input, GetCore(), System::eSystemInput);
    input->AddHandler(mInputMsgHandler);

#ifdef TACTWEAK

    mTweakMenu = GetCore()->GetTweak().Create("Menu");
    mTweakMenu->SetIconified(true);
    mTweakMenu->SetPosition(10,10);
    mTweakMenu->SetSize(270,400);
    std::string speedLabel("label='Camera speed' ");
    speedLabel += "min=0 ";
    speedLabel += "step=0.01";
    mTweakMenu->AddVar("MovementSpeed", speedLabel, TW_TYPE_FLOAT, 
      &mCameraMovementSpeed);
    mTweakMenu->AddVar("InvertH", 
      "label='Inverted horizontal'", TW_TYPE_BOOLCPP, 
      &mCamInvertedHorizControls);
    mTweakMenu->AddVar("InvertV", 
      "label='Inverted vertical'", TW_TYPE_BOOLCPP, 
      &mCamInvertedVertiControls);
    mTweakMenu->AddComment("Controls0", "WASD --> move");
    mTweakMenu->AddComment("Controls1", "EQ --> up / down");
    mTweakMenu->AddComment("Controls2", "Scroll --> zoom");
    mTweakMenu->AddComment("Controls3", "Left-click --> select");
    mTweakMenu->AddComment("Controls4", "Right-click + drag --> look");

    mTweakMenu->AddButton(
      "OpenLevel", "Open Level", CallbackOpenLevel, this);
    mTweakMenu->AddButton(
      "SaveLevel", "Save Level", CallbackSaveLevel, this);
    mTweakMenu->AddButton(
      "SaveLevelAS", "Save Level As", CallbackSaveLevelAs, this);
    mTweakMenu->AddButton(
      "CreateEntity", "Create Entity", CallbackCreateEntity, this);
    mTweakMenu->AddVar(
      "Entity to select", "label='Entity to select'", 
      TW_TYPE_INT32, &mIDToSelect);
    mTweakMenu->AddButton(
      "Select Entity", "Select Entity", CallbackOnSelectEntity, this);
    //mTweakMenu->AddButton(
    //"DeleteEntity", "Delete Selected", )
    mTweakMenu->AddComment("Entities", "Entities");


#endif

  
  }

} // Tac