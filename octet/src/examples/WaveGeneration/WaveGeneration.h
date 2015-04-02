////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "AntTweakBar\AntTweakBar.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "WavePlane.h"
#include "File.h"



namespace octet {
    void TW_CALL MakeTrue(void *v)
    {
        bool* bp = (bool*)v;
        *bp = true;
    }
    struct TweakWave
    {
        TweakWave(): wav(0.00001f),ampl(0.00001f),speed(0),q(0),weight(0),dirX(1),padding(0),dirZ(0)
        {}
        //returns the weight of the wave
        float Share(GerstnerWave* g)
        {
            if (wav != g->GetWavelength())
            {
                g->SetWavelength(wav);
            }
            if (ampl != g->GetAmplitude())
            {
                g->SetAmplitude(ampl);
            }
            if (speed != g->GetSpeed())
            {
                g->SetSpeed(speed);
            }
            if (q != g->GetQ())
            {
                g->SetQ(q);
            }
            bvec2 v = vec2(dirX, dirZ) != g->GetDir();
            if (v.x()==-1||v.y()==-1)
            {
                g->SetDirection(vec2(dirX, dirZ));
            }
            return weight;
        }
        //intializes values sparingly
        void Init(float w, float a, float s, float dx, float dy, float q, float wght)
        {
            wav = w;
            ampl = a;
            speed = s;
            q = q;
            dirX = dx;
            dirZ = dy;
            weight = wght;
        }
        float wav;
        float ampl;
        float speed;
        float q;
        float weight;
        float dirX;
        //used to align to fit with anttweakbars vec3 type
        float padding;
        float dirZ;
        
    };
  /// Scene containing a box with octet.
    class WaveGeneration : public app {
        // scene for drawing box
        ref<visual_scene> app_scene;
        material *red;
        WavePlane p;
        TwBar* bar_;
        int numWaves;

        bool shouldSave;
        bool makeDefault;
        float maxLoopTime;
        float t;//time variable

        std::vector<TweakWave> tweakWaves;
    public:
        void KeyboardInputControl()
        {
            if (is_key_down(key::key_esc))
            {
                exit(1);
            }
            float speed = 1;
            if (is_key_down(key::key_shift))
            {
                speed = 0.08;
            }
            if (is_key_down('W'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, speed, 0);
            }
            if (is_key_down('S'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -speed, 0);
            }
            if (is_key_down('E'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -speed);
            }
            if (is_key_down('Q'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, speed);
            }
            if (is_key_down('A'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(-speed, 0, 0);
            }
            if (is_key_down('D'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(speed, 0, 0);
            }
            if (is_key_down('Z'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateX(-speed);
            }
            if (is_key_down('X'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateX(speed);
            }
            if (is_key_down('C'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateY(speed);
            }
            if (is_key_down('V'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateY(-speed);
            }
      }
        void KeyboardTwiddleControl()
        {
            static bool wireframe = false;
            if (is_key_going_down(' '))
            {
                wireframe = !wireframe;
                if (wireframe)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        
        
        }
        void MouseInputControl()
        {
            int mX = 0, mY = 0;
            get_mouse_pos(mX, mY);
            TwMouseMotion(mX, mY);
            if (is_key_going_down(key_lmb))
            {
                TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
            }
            if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
            {
                TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
            }
        }
        void HackyKeyboardTranslation()
        {
            if (is_key_going_down('1'))
            {
                TwKeyPressed('1', 0);
            }
            if (is_key_going_down('2'))
            {
                TwKeyPressed('2', 0);
            }
            if (is_key_going_down('3'))
            {
                TwKeyPressed('3', 0);
            }
            if (is_key_going_down('4'))
            {
                TwKeyPressed('4', 0);
            }
            if (is_key_going_down('5'))
            {
                TwKeyPressed('5', 0);
            }
            if (is_key_going_down('6'))
            {
                TwKeyPressed('6', 0);
            }
            if (is_key_going_down('7'))
            {
                TwKeyPressed('7', 0);
            }
            if (is_key_going_down('8'))
            {
                TwKeyPressed('8', 0);
            }
            if (is_key_going_down('9'))
            {
                TwKeyPressed('9', 0);
            }
            if (is_key_going_down('0'))
            {
                TwKeyPressed('0', 0);
            }
            //for some reason 190 is the key code for '.'
            //46 is the ascii code for '.'
            if (is_key_going_down(190))
            {
                TwKeyPressed(46, 0);
            }
            //control values using ascii found here
            //http://www.asciitable.com/
            if (is_key_going_down(key_backspace))
            {
                TwKeyPressed(8, 0);
            }

        }


        void InitSkydome()
        {
            material *skydome = new material(new image("assets/skydome.jpg"));
            app_scene->add_mesh_instance(new mesh_instance(new scene_node(), new mesh_sphere(),
                skydome));
            app_scene->get_mesh_instance(2)->get_node()->access_nodeToParent().scale(1000, 1000, 1000);
        
        }
        void CreateScene()
        {
            param_shader* sh = new param_shader("shaders/gwave.vs", "shaders/wave.fs");

            red = new material(new image("assets/sea.gif"), 0, sh);
            red->add_uniform(NULL, atom_waves, GL_FLOAT, 100, param::stage_vertex);
            red->add_uniform(NULL, atom_numWaves, GL_INT, 1, param::stage_vertex);
            red->add_uniform(NULL, atom_time, GL_FLOAT, 1, param::stage_vertex);

            material *yellow = new material(vec4(1, 1, 0, 0));



            mesh *box = p.GetMesh();
            scene_node *node = new scene_node();
            app_scene->add_child(node);
            app_scene->add_mesh_instance(new mesh_instance(node, box, red));
            app_scene->add_mesh_instance(new mesh_instance(app_scene->get_light_instance(0)->get_node(),
                new mesh_box(vec3(1, 1, 1)), yellow));

        }
        bool LoadWaves()
        {
            File f;
            if (!f.OpenRead("waves.txt"))
            {
                printf("Could not find wave file, will use default waves\n");
                return false;
            }
            std::string read;
            TweakWave* wave = NULL;
            float q = 0;
            float ampl = 0;
            float speed = 0;
            float wavlen = 0;
            float xDir = 0, yDir = 0;
            float weight = 1;
            while (f.GetString(&read))
            {
                if (read == "Wave")
                {
                    if (numWaves == 19)
                    {
                        printf("Maximum number of waves reached\n");
                            return true;
                    }
                    wave = &tweakWaves[numWaves];
                }
                if (read == "Wavelength"&&wave != NULL)
                {
                    f.GetFloat(&wavlen);
                }
                if (read == "Amplitude"&&wave != NULL)
                {
                    f.GetFloat(&ampl);
                }
                if (read == "Speed"&&wave != NULL)
                {
                    f.GetFloat(&speed);
                }
                if (read == "Q"&&wave != NULL)
                {
                    f.GetFloat(&q);
                }
                if (read == "Direction"&&wave != NULL)
                {
                    f.GetFloat(&xDir);
                    f.GetFloat(&yDir);
                }
                if (read == "Weight"&&wave != NULL)
                {
                    f.GetFloat(&weight);
                }
                if (read == "END"&&wave!=NULL)
                {
                    if (!q || !ampl || !speed || !wavlen || !xDir || !yDir ||
                        !weight)
                    {
                        printf("Warning, value initialised in file to 0\n");
                    }
                    wave->Init(wavlen, ampl, speed, xDir, yDir, q,weight);
                    GerstnerWave* g = new GerstnerWave();
                    p.AddWave(g, wave->Share(g));
                    wave = NULL;
                    numWaves++;
                    q = 0;
                    ampl = 0;
                    speed = 0;
                    wavlen = 0;
                    xDir = 0, yDir = 0;
                    weight = 1;
                }
            }
            if (numWaves > 0)
            {
                return true;
            }
            return false;
        }
        bool SaveWaves()
        {
            File f;
            if (!f.OpenWrite("waves.txt",true))
            {
                printf("Error: Could not find or create save file\n");
                assert(0);
            }
           
            for (int i = 0; i < numWaves; ++i)
            {
                GerstnerWave* wave = p.GetWave(i);
                f.WriteString("Wave\n");
                f.WriteString("Wavelength\n");
                f.WriteFloat(wave->GetWavelength());
                f.WriteString("\n");
                f.WriteString("Amplitude\n");
                f.WriteFloat(wave->GetAmplitude());
                f.WriteString("\n");
                f.WriteString("Speed\n");
                f.WriteFloat(wave->GetSpeed());
                f.WriteString("\n");
                f.WriteString("Q\n");
                f.WriteFloat(wave->GetQ());
                f.WriteString("\n");
                f.WriteString("Direction\n");
                f.WriteFloat(wave->GetDir().x());
                f.WriteString("\n");
                f.WriteFloat(wave->GetDir().y());
                f.WriteString("\n");
                f.WriteString("Weight\n");
                f.WriteFloat(p.GetWeight(i));
                f.WriteString("\n");
                f.WriteString("END\n");

            }
            f.Close();
            return false;
        }
        void DefaultWaves()
        {
            numWaves = 5;
            tweakWaves[0].Init(16, 0.4f, 1, 0, 1, 2.0f,2.0f);
            tweakWaves[1].Init(44, 1.4, 4, 1, 0, 2.0f, 0.5f);
            tweakWaves[2].Init(32, 1, 2, -0.7, -0.7, 2.0f, 1);
            tweakWaves[3].Init(100, 1.4, 3, 0.7, 0.3, 1.0f, 1);
            tweakWaves[4].Init(50, 1.4, 2, 0, -1, 2.0f, 1);
        }

        void SyncWaves()
        {
            for (int i = 0; i < numWaves; ++i)
            {
                if (numWaves>p.GetNumActiveWaves())
                {
                    if (numWaves > p.GetNumWaves())
                    {
                        for (int i = p.GetNumWaves(); i < numWaves; ++i)
                        {
                            p.AddWave(new GerstnerWave(), 0);
                        }
                    }
                    for (int i = p.GetNumActiveWaves(); i < numWaves; ++i)
                    {
                        char c[30];
                        sprintf(c, "WaveBar/Wave%d visible=true", i);
                        TwDefine(c);
                    }
                }
                else if (numWaves < p.GetNumActiveWaves())
                {
                    for (int i = p.GetNumActiveWaves()-1; i>=numWaves; --i)
                    {
                        char c[30];
                        sprintf(c, "WaveBar/Wave%d visible=false", i);
                        TwDefine(c);
                    }
                    
                
                }
                //this line does alot, it gets a wave and shares the information with it
                //if neccessary, it then re-inits the weight.
                p.SetWeight(i, tweakWaves[i].Share(p.GetWave(i)));
            }
            p.SetNumActiveWaves(numWaves);
        }
        
        
    /// this is called when we construct the class before everything is initialised.
    WaveGeneration(int argc, char **argv) : app(argc, argv) {
    }

    
    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->set_far_plane(1000000.0f);
      tweakWaves.resize(20);
      p.Init(100,100,1.0f);
      numWaves = 0;
      shouldSave = false;
      makeDefault = false;
      maxLoopTime = 0.0f;
      t = 0.0f;
      if (!LoadWaves())
      {
          DefaultWaves();
      }
      CreateScene();

      TwInit(TW_OPENGL, NULL);
      TwWindowSize(768, 732);//minus 30 because "I dont know why"
      
      bar_ = TwNewBar("WaveBar");

      TwDefine("WaveBar refresh=0.2");
      TwAddVarRO(bar_, "Time", TW_TYPE_FLOAT, &t, "help ='Simulation time for the waves'");
      TwAddVarRW(bar_, "Loop duration", TW_TYPE_FLOAT, &maxLoopTime, "min=0 help='Changes how long the simulation will take to loop, a value of 0 indicating that it will not loop'");

      TwAddVarRW(bar_, "NumWaves", TW_TYPE_INT32, &numWaves,
          "label ='NumWaves' min=0 max=20 help='Changes the number of active waves in the scene.'");

      TwStructMember waveMembers[]=
      {
          { "Wavelength", TW_TYPE_FLOAT, offsetof(TweakWave, wav), "min=0.0001 help='Wavelength of wave.'" },
          { "Amplitude", TW_TYPE_FLOAT, offsetof(TweakWave, ampl), "step=0.1 help='Height of wave.'" },
          { "Speed", TW_TYPE_FLOAT, offsetof(TweakWave, speed), "step=0.1 help='Speed of wavefront.'" },
          { "Q value", TW_TYPE_FLOAT, offsetof(TweakWave, q), " help='Value controlling peak of waves.'" },
          { "Direction", TW_TYPE_DIR3F, offsetof(TweakWave, dirX), "axisx=-x help='Direction of the traveling wave'" },
          { "Weight", TW_TYPE_FLOAT, offsetof(TweakWave, weight), "min=0 help='Weighting of this waves contribution, use to fade out effects.'" },
      };
      TwType waveType = TwDefineStruct("Wave", waveMembers, 6, sizeof(float) * 8, NULL, NULL);

      for (int i = 0; i < 20; ++i)
      {
          char c[30];
          sprintf(c, "Wave%d", i);
          TwAddVarRW(bar_, c, waveType, &tweakWaves[i], " group='Edit waves'");
          if (i >= numWaves)
          {
              sprintf(c, "WaveBar/Wave%d visible=false", i);
              TwDefine(c);
          }
      }

      TwAddButton(bar_, "Save Waves", MakeTrue, &shouldSave, " help='Save your current wave loadout'");
      TwAddButton(bar_, "Default Waves", MakeTrue, &makeDefault, "help='Reset the waves to the default'");
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
      

      KeyboardInputControl();
      KeyboardTwiddleControl();
      MouseInputControl();
      HackyKeyboardTranslation();
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      if (shouldSave)
      {
          shouldSave = false;
          SaveWaves();
      }
      if (makeDefault)
      {
          makeDefault = false;
          DefaultWaves();
      }
      SyncWaves();
      
      app_scene->get_light_instance(0)->get_node()->access_nodeToParent()[3] = vec4(0,100,0);
      app_scene->get_light_instance(0)->get_node()->access_nodeToParent().lookat(vec3(0, 0, 0),vec3(0,0,1));
      p.SetNumActiveWaves(numWaves);
      p.GenerateWaves(t+=1.0f/30);
      if (t > maxLoopTime&&maxLoopTime>0.000000001f)
      {
          t = 0;
      }
      float* f;
      red->set_uniform(red->get_param_uniform(atom_waves), f, p.GetShaderInfo(&f));
      red->set_uniform(red->get_param_uniform(atom_time), &t, sizeof(float));
      red->set_uniform(red->get_param_uniform(atom_numWaves), &numWaves, sizeof(int));
      // draw the scene
      app_scene->render((float)vx / vy);
      TwDraw();
    }
  };
}
