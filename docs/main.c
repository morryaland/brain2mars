#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <stdio.h>

#define ZOOM 50.0f

static void set_draw_color(b2HexColor color, SDL_Renderer *renderer)
{
  unsigned char red = (color >> 16) & 0xFF;
  unsigned char green = (color >> 8) & 0xFF;
  unsigned char blue = color & 0xFF;
  SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
}
void SDL_RenderCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
const int32_t diameter = (radius * 2);
int32_t x = (radius - 1);
int32_t y = 0;
int32_t tx = 1;
int32_t ty = 1;
int32_t error = (tx - diameter);
while (x >= y) {
SDL_RenderPoint(renderer, centreX + x, centreY - y);
SDL_RenderPoint(renderer, centreX + x, centreY + y);
SDL_RenderPoint(renderer, centreX - x, centreY - y);
SDL_RenderPoint(renderer, centreX - x, centreY + y);
SDL_RenderPoint(renderer, centreX + y, centreY - x);
SDL_RenderPoint(renderer, centreX + y, centreY + x);
SDL_RenderPoint(renderer, centreX - y, centreY - x);
SDL_RenderPoint(renderer, centreX - y, centreY + x);
  if (error <= 0) {
  	++y;
  	error += ty;
  	ty += 2;
  }
  if (error > 0) {
  	--x;
  	tx += 2;
  	error += (tx - diameter);Uint64 nowf, lastf = 0;
  }
}
}


void draw_solid_polygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
  SDL_Renderer *renderer = context;
  b2Vec2 pv[vertexCount + 1] = {};
  for (int i = 0; i < vertexCount; i++) {
    pv[i] = b2Mul(b2TransformPoint(transform, vertices[i]), (b2Vec2){ZOOM, ZOOM});
  }
  pv[vertexCount] = pv[0];
  set_draw_color(color, renderer);
  SDL_RenderLines(renderer, (SDL_FPoint*)pv, vertexCount + 1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void draw_solid_circle(b2Transform transform, float radius, b2HexColor color, void *context)
{
  SDL_Renderer *renderer = context;
  b2Vec2 center = b2Mul(transform.p, (b2Vec2){ZOOM, ZOOM});
  set_draw_color(color, renderer);
  SDL_RenderCircle(renderer, center.x, center.y, radius * ZOOM);
  b2Vec2 rp = b2Mul(b2TransformPoint(transform, (b2Vec2){radius, 0}), (b2Vec2){ZOOM, ZOOM});
  SDL_RenderLine(renderer, center.x, center.y, rp.x, rp.y);
  SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 1.0f);
}

void draw_segment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
  SDL_Renderer *renderer = context;
  SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, 1);
  p1 = b2Mul(p1, (b2Vec2){ZOOM, ZOOM});
  p2 = b2Mul(p2, (b2Vec2){ZOOM, ZOOM});
  SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
  SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 1);
}

int main()
{
    // Переменные для управления временем кадров
    Uint64 nowf, lastf = 0;
    
    // Переменные SDL для окна, рендерера и событий
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event e;
    
    // Массив для хранения идентификаторов динамических тел (кубов) и счетчик
    b2BodyId cube_body[100];
    int cube_c = 0;

    // Идентификатор мира Box2D и структура для отладочной отрисовки
    b2WorldId world_id;
    b2DebugDraw dd;

    // Инициализация SDL подсистем видео и событий :cite[2]:cite[6]
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    
    // Создание окна и рендерера SDL :cite[3]:cite[7]
    SDL_CreateWindowAndRenderer("box2d", 800, 600, 0, &window, &renderer);
    SDL_ShowWindow(window);

    // Настройка отладочной отрисовки Box2D :cite[4]:cite[8]:cite[9]
    dd = b2DefaultDebugDraw();
    dd.context = renderer;  // Передача рендерера в контекст
    dd.DrawSolidPolygonFcn = draw_solid_polygon;
    dd.DrawSolidCircleFcn = draw_solid_circle;
    dd.DrawSegmentFcn = draw_segment;
    dd.drawJoints = true;   // Включение отрисовки соединений

    // Создание мира Box2D с нулевой гравитацией :cite[1]:cite[9]
    b2WorldDef w_def = b2DefaultWorldDef();
    w_def.gravity = (b2Vec2){0, 0};
    world_id = b2CreateWorld(&w_def);

    // Создание статического тела для пола
    b2BodyDef floor_body_def = b2DefaultBodyDef();
    b2BodyId floor_id = b2CreateBody(world_id, &floor_body_def);
    
    // Настройка свойств формы для пола
    b2ShapeDef floor_shape_def = b2DefaultShapeDef();
    floor_shape_def.material = b2DefaultSurfaceMaterial();
    floor_shape_def.enableHitEvents = true;  // Включение событий столкновений
    
    // Определение сегментов для пола и стен
    b2Segment floor_segment = {{2, 10}, {15, 10}};
    b2Segment wall_segment = {{0, 0}, {0, 598/ZOOM}};
    b2Segment wall1_segment = {{0, 0}, {798/ZOOM, 0}};
    b2Segment wall2_segment = {{0, 598/ZOOM}, {798/ZOOM, 598/ZOOM}};
    b2Segment wall3_segment = {{798/ZOOM, 0}, {798/ZOOM, 598/ZOOM}};
    
    // Создание форм для пола и стен
    b2ShapeId floor_shape_id = b2CreateSegmentShape(floor_id, &floor_shape_def, &floor_segment);
    b2CreateSegmentShape(floor_id, &floor_shape_def, &wall_segment);
    b2CreateSegmentShape(floor_id, &floor_shape_def, &wall1_segment);
    b2CreateSegmentShape(floor_id, &floor_shape_def, &wall2_segment);
    b2CreateSegmentShape(floor_id, &floor_shape_def, &wall3_segment);

    // Создание сенсорного триггера
    b2BodyDef triger_body_def = b2DefaultBodyDef();
    b2BodyId triger_id = b2CreateBody(world_id, &floor_body_def);
    b2ShapeDef triger_shape_def = b2DefaultShapeDef();
    triger_shape_def.isSensor = true;           // Делаем форму сенсором
    triger_shape_def.enableSensorEvents = true; // Включение событий сенсора
    floor_shape_def.material = b2DefaultSurfaceMaterial();
    b2Segment triger_segment = {{12, 3}, {15, 6}};
    b2CreateSegmentShape(triger_id, &triger_shape_def, &triger_segment);

    // Настройка определения для динамических тел (кубов и кругов)
    b2BodyDef cube_body_def = b2DefaultBodyDef();
    cube_body_def.type = b2_dynamicBody;     // Динамическое тело
    cube_body_def.linearDamping = 0.5f;      // Линейное демпфирование
    
    b2BodyDef circle_body_def = b2DefaultBodyDef();
    circle_body_def.type = b2_dynamicBody;   // Динамическое тело
    
    // Настройка свойств форм для кубов и кругов
    b2ShapeDef cube_shape_def = b2DefaultShapeDef();
    cube_shape_def.enableSensorEvents = true;
    cube_shape_def.enableHitEvents = true;
    cube_shape_def.material = b2DefaultSurfaceMaterial();
    cube_shape_def.filter.categoryBits = 2;
    cube_shape_def.filter.maskBits = ~2;
    
    b2ShapeDef circle_shape_def = b2DefaultShapeDef();
    circle_shape_def.material = b2DefaultSurfaceMaterial();
    circle_shape_def.enableSensorEvents = true;
    
    // Создание полигона (квадрата) и круга
    b2Polygon cube_poly = b2MakeSquare(0.5f);
    b2Circle circle = {.center = {}, .radius = 0.5f};

    // Основной игровой цикл
    for(;;) {
        nowf = SDL_GetTicks();
        Uint64 deltaf = nowf - lastf;
        
        // Ограничение частоты обновления (примерно 60 FPS)
        if (deltaf > 1000/60.0f) {
            lastf = nowf;
            
            // Обработка событий SDL :cite[6]:cite[7]
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        return 0;  // Завершение программы при закрытии окна
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            // Создание куба при левом клике мыши
                            cube_body[cube_c] = b2CreateBody(world_id, &cube_body_def);
                            b2Body_SetTransform(cube_body[cube_c], 
                                (b2Vec2){e.button.x / ZOOM, e.button.y / ZOOM}, 
                                b2MakeRot(0));
                            b2CreatePolygonShape(cube_body[cube_c], &cube_shape_def, &cube_poly);
                            b2Vec2 cpos = b2Body_GetPosition(cube_body[cube_c]);
                            printf("cube pos x: %f y: %f\n", cpos.x, cpos.y);
                            cube_c++;
                        }
                        else {
                            // Создание круга при правом клике мыши
                            b2BodyId circle_id = b2CreateBody(world_id, &circle_body_def);
                            b2Body_SetTransform(circle_id, 
                                (b2Vec2){e.button.x / ZOOM, e.button.y / ZOOM}, 
                                b2MakeRot(0));
                            b2CreateCircleShape(circle_id, &circle_shape_def, &circle);
                            b2Vec2 cpos = b2Body_GetPosition(circle_id);
                            printf("circle pos x: %f y: %f\n", cpos.x, cpos.y);
                        }
                        break;
                    default:
                        break;
                }
            }
            
            // Обработка клавиш для применения сил к кубам
            float left = 0;
            float right = 0;
            const bool *keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_LEFT])
                left = 5;
            if (keystate[SDL_SCANCODE_RIGHT])
                right = 5;
            
            // Применение сил к кубам
            if (left || right) {
                for (int i = 0; i < cube_c; i++) {
                    if (!b2Body_IsValid(cube_body[i]))
                        continue;
                    b2Vec2 force = b2RotateVector(b2Body_GetRotation(cube_body[i]), 
                        (b2Vec2){0, left + right});
                    b2Vec2 pos = b2Body_GetWorldPoint(cube_body[i], 
                        (b2Vec2){0 - SDL_min(left, 0.5) + SDL_min(right, 0.5), -0.5});
                    b2Body_ApplyForce(cube_body[i], force, pos, true);
                }
            }
            
            // Шаг симуляции Box2D :cite[1]:cite[9]
            b2World_Step(world_id, 1/60.0f, 4);
            
            // Обработка событий сенсоров :cite[9]
            b2SensorEvents sensorEvents = b2World_GetSensorEvents(world_id);
            for (int i = 0; i < sensorEvents.beginCount; ++i) {
                b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
                printf("body type: %d enter into %d\n", 
                    b2Shape_GetType(beginTouch->visitorShapeId), 
                    b2Shape_GetType(beginTouch->sensorShapeId));
            }
            for (int i = 0; i < sensorEvents.endCount; ++i) {
                b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;
                printf("body type: %d exit from %d\n", 
                    b2Shape_GetType(endTouch->visitorShapeId), 
                    b2Shape_GetType(endTouch->sensorShapeId));
            }
            
            // Обработка контактных событий :cite[9]
            b2ContactEvents contact = b2World_GetContactEvents(world_id);
            for (int i = 0; i < contact.hitCount; ++i) {
                b2ContactHitEvent *hit = contact.hitEvents + i;
                if (b2Shape_GetType(hit->shapeIdA) == b2_polygonShape) {
                    // Остановка и применение импульса к полигональным телам
                    b2Body_SetLinearVelocity(b2Shape_GetBody(hit->shapeIdA), b2Vec2_zero);
                    b2Body_ApplyLinearImpulseToCenter(b2Shape_GetBody(hit->shapeIdA), 
                        b2MulSV(-2, hit->normal), false);
                }
                else {
                    b2Body_SetLinearVelocity(b2Shape_GetBody(hit->shapeIdB), b2Vec2_zero);
                    b2Body_ApplyLinearImpulseToCenter(b2Shape_GetBody(hit->shapeIdB), 
                        b2MulSV(2, hit->normal), false);
                }
            }

            SDL_RenderClear(renderer);
            b2World_Draw(world_id, &dd);  // Отладочная отрисовка Box2D
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            int rays = 100;
            b2QueryFilter filter = b2DefaultQueryFilter();
            filter.maskBits = ~2;
                for (int j = 0; j < cube_c; j++) {
                  b2Vec2 cubepos = b2Body_GetPosition(cube_body[j]);
                  b2Rot cuberot = b2Body_GetRotation(cube_body[j]);
            for (int i = 0; i < rays; i++) {
              b2Vec2 target = b2RotateVector(b2MakeRot(b2Rot_GetAngle(cuberot) + (i / (rays/360.0f)) * (M_PI / 180.0f)), (b2Vec2){300/2/ZOOM, 0});
              b2RayResult data = b2World_CastRayClosest(world_id, cubepos, target, filter);
              if (data.hit) {
                if (b2Shape_GetType(data.shapeId) == b2_polygonShape)
                  continue;
                SDL_RenderLine(renderer, cubepos.x * ZOOM, cubepos.y * ZOOM, ZOOM * data.point.x, ZOOM * data.point.y);
              } else {                                                     
                SDL_RenderLine(renderer, cubepos.x * ZOOM, cubepos.y * ZOOM, cubepos.x * ZOOM + ZOOM * target.x, cubepos.y * ZOOM + ZOOM * target.y);
              }
            }
                }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderPresent(renderer);
        }
        
        // Задержка для поддержания частоты кадров
        SDL_Delay(SDL_max(1000/60.0f - deltaf, 0));
    }
    
    return 0;
}
