#ifndef ENTITY
#define ENTITY

#include <raylib.h>
#include <raymath.h>

class Tile {
public:
    Rectangle loc_sprite_sheet;
    bool has_collision;
};

class Projectile {
public:
    Color color;
    float radius;
    Vector2 position;
    Vector2 direction;
    float speed;
    bool friendly;
    bool exists;
};

class Bullet :  public Projectile {
public:

    //constructor
    Bullet(Color col, float rad, Vector2 pos, Vector2 dir, float sped, bool friends, bool truth) : Projectile{col, rad, pos, dir, sped, friends, truth} {}
};

class Boomerang :  public Projectile {
public:
    float collision_rad;
    //constructor
    Boomerang(Color col, float rad, Vector2 pos, Vector2 dir, float sped, bool friends, bool truth) : Projectile{col, rad, pos, dir, sped, friends, truth} {
        collision_rad = 5.0f;
    }
};

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float health;
    float radius;
    float speed;

    void CheckTileCollision(const Rectangle tile) {
        Vector2 closest_point;
        closest_point.x = Clamp(position.x, tile.x, tile.x + tile.width);
        closest_point.y = Clamp(position.y, tile.y, tile.y + tile.height);
        float distance = sqrt((position.x - closest_point.x) * (position.x - closest_point.x) + (position.y - closest_point.y) * (position.y - closest_point.y));

        if (distance < radius) {
            Vector2 col_norm;
            col_norm = Vector2Normalize(Vector2Subtract(position, closest_point));
            float overlap = radius - distance;
            Vector2 movement = Vector2Scale(col_norm, overlap);
            position = Vector2Add(position, movement);
        }

    }

};



class Bezier {
    public:
        std::vector<int> Pasc_tracker;
        std::vector<int> Pasc_list = {1, 1};
        int degree_check = 1;

        struct Control { //Major control points struct
            Vector2 Coords;
            float radius;
            Color color;
        };

        Control contr_points; //Control container to push back to Controller
        std::vector<Control> Controller; //Container for major control points 
        std::vector<Vector2> Step_coords; //Curve flow coordinates container
        Vector2 inst; //Create empty coordinates for all the curve flow coordinates based on steps 

        std::vector<int> bez_placer; //Repository for tangent drawing point start
        std::vector<Vector2> tang_placer; //Repository for tangent drawing point end
        std::vector<Vector2> perp_placer; //Repository for perpendicular drawing point end

        int degree_input; //Degree of the curve
        int control_input; //Number of control points
        int tangent_input; //Number of tangents in the curve
        int step_input; //Number of points in the curve
        float tang_step;

        bool isActive;

        Bezier(int degree, int control, int tangent, int step, float rad, Color col) : degree_input(degree), control_input(control), tangent_input(tangent), step_input(step), contr_points{{0, 0}, rad, col} {
        
            Recurse(Pasc_list, degree, degree_check);
            //Instantiate container for steps coordinates
            for (int i = 0; i < step_input; i++) {

                Step_coords.push_back(inst);        

            }

            //Get the distance for spawning the tangents based on input
            tang_step = step_input / tangent_input;
            
            for (float i = 0; i <= step_input; i += tang_step) 
            {                
                bez_placer.push_back(round(i));
            }

            for (int i = 0; i < tangent_input; i++)
            {
                tang_placer.push_back(inst);
                perp_placer.push_back(inst);
            }

            for (int i=0; i < control; i++) {
                CREATE({0, 0});
            }

        };

        void Update() {

            //Setup the equation for the step coordinates
            // tang_updater = 0;
            for (float e = 0; e < step_input; e++) {
                int rev_expo = 0;
                Vector2 equation = {0, 0};
                float steps = (e/step_input);
                for (int i = degree_input; i >= 0; i--) {
                
                    equation = Vector2Add(equation, Vector2Scale((Controller[rev_expo].Coords), Pasc_list[rev_expo] * std::pow(1 - steps, i) * std::pow(steps, rev_expo)));
                    rev_expo++;

                }

                Step_coords[e] = equation;

                // if (bez_placer[tang_updater] == e) 
                //     {
                //     // tang_equation_result = {0, 0};
                //     rev_expo = 0;
                //     for (int i = degree_input - 1; i >= 0; i--) 
                //         {
                //         // tang_equation_result = Vector2Add(tang_equation_result, Vector2Scale((Vector2Subtract(Controller[rev_expo + 1].Coords, Controller[rev_expo].Coords)), Pasc_tracker[rev_expo] * std::pow(1 - steps, i) * std::pow(steps, rev_expo) * degree_input));
                //         rev_expo++;

                //         }   
                    // tang_equation_result = Vector2Normalize(tang_equation_result);
                    // tang_equation_result = Vector2Scale(tang_equation_result, tang_stretcher);
                    // perp_placer[tang_updater] = {tang_equation_result.y * -1, tang_equation_result.x};
                    // tang_placer[tang_updater] = tang_equation_result;
                    // tang_updater++;

                    }

               };

        void Draw() {
            // float deltaTime = GetFrameTime();
            //Drawing the curve
            for (int i = 0; i < step_input - 1; i++) {
                DrawLine(Step_coords[i].x, Step_coords[i].y, Step_coords[i + 1].x, Step_coords[i + 1].y, WHITE);
                
                }
            //Drawing the controllers
            for (int i = 0; i < Controller.size(); i++) {
                DrawCircle(Controller[i].Coords.x, Controller[i].Coords.y, Controller[i].radius, Controller[i].color);
                }
        }

        void CREATE(Vector2 points) {
            Bezier::Control control; // Create an instance of the Control struct

            // Initialize the fields of the control struct
            control.Coords = points;
            control.radius = contr_points.radius;
            control.color = contr_points.color;
            
            Controller.push_back(control);
        }

        int Recurse(std::vector<int> Pasc, int degree, int degree_check) {
        //Recursive function to get Pascal's coefficients depending on the degree

            //Once the degree checker has matched with the degree, end the recursion
            if (degree_check == degree)
            {
                return 0;
            }

            else
            {
                Pasc_tracker.clear();
                std::copy(Pasc_list.begin(), Pasc_list.end(), std::back_inserter(Pasc_tracker));
                Pasc_list.clear();
            
                for (int i = 0; i < Pasc.size() + 1; i++)
                {
                    //pushes back 1 because index doesn't first exist
                    if (i == 0)
                    {
                        Pasc_list.push_back(1);
                    }

                    //pushes back 1 because index doesn't second exist
                    else if(i == Pasc.size())
                    {
                        Pasc_list.push_back(1);
                    }

                    //pushes back added value like in Pascal's 
                    else    
                    {
                        int add = Pasc[i - 1] + Pasc[i];
                        Pasc_list.push_back(add);  
                    }
                    }
            
                return Recurse(Pasc_list, degree, degree_check + 1);
            }
        }
};

#endif 