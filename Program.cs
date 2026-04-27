using Raylib_cs;
using System.Numerics;

class Player
{
    public System.Numerics.Vector3 Position = new System.Numerics.Vector3(0.0f, 2.0f, 4.0f);
    public System.Numerics.Vector3 Velocity = new System.Numerics.Vector3(0, 0, 0);
    public Raylib_cs.Camera3D Camera;
    public float Speed = 5.0f;

    public Player()
    {
        Camera = new Raylib_cs.Camera3D
        {
            position = Position,
            target = new System.Numerics.Vector3(0.0f, 2.0f, 0.0f),
            up = new System.Numerics.Vector3(0.0f, 1.0f, 0.0f),
            fovy = 60.0f,
            projection = Raylib_cs.CameraProjection.CAMERA_PERSPECTIVE
        };
    }

    public void Update()
    {
        if (Raylib.IsKeyDown(Raylib_cs.KeyboardKey.KEY_W)) Position.Z -= Speed * Raylib.GetFrameTime();
        if (Raylib.IsKeyDown(Raylib_cs.KeyboardKey.KEY_S)) Position.Z += Speed * Raylib.GetFrameTime();
        if (Raylib.IsKeyDown(Raylib_cs.KeyboardKey.KEY_A)) Position.X -= Speed * Raylib.GetFrameTime();
        if (Raylib.IsKeyDown(Raylib_cs.KeyboardKey.KEY_D)) Position.X += Speed * Raylib.GetFrameTime();
        Camera.position = Position;
    }

    public void LookAround()
    {
        System.Numerics.Vector2 mouseDelta = Raylib.GetMouseDelta();
        Camera.target.X += mouseDelta.X * 0.005f;
        Camera.target.Y += mouseDelta.Y * 0.005f;
    }
}

class Program
{
    static void Main()
    {
        // Инициализация окна и аудио
        Raylib.InitWindow(1280, 720, "CS3 - Counter-Strike 3D");
        Raylib.InitAudioDevice();
        Raylib.SetTargetFPS(60);

        // Отключение курсора для FPS-камеры
        Raylib.DisableCursor();

        // Настройка камеры (Первый человек)
        Player player = new Player();

        // Загрузка звука выстрела
        Raylib_cs.Sound shootSound = Raylib.LoadSound("resources/sounds/shot.wav");

        // Загрузка модели оружия (из CS 1.6, конвертируй .mdl в .obj если нужно)
        Raylib_cs.Model weaponModel = Raylib.LoadModel("resources/models/ak47.obj");

        // Основной игровой цикл
        while (!Raylib.WindowShouldClose())
        {
            player.Update();
            player.LookAround();

            // Проверка выстрела (левая кнопка мыши)
            if (Raylib.IsMouseButtonPressed(Raylib_cs.MouseButton.MOUSE_BUTTON_LEFT))
            {
                Raylib.PlaySound(shootSound);
            }

            // Рисование
            Raylib.BeginDrawing();
            Raylib.ClearBackground(new Raylib_cs.Color(0, 0, 139, 255));

            Raylib.BeginMode3D(player.Camera);
            // Рисуем "шахматную" сетку (пол)
            Raylib.DrawGrid(20, 1.0f);
            // Рисуем куб в центре
            Raylib.DrawCube(new System.Numerics.Vector3(0.0f, 0.5f, 0.0f), 1.0f, 1.0f, 1.0f, new Raylib_cs.Color(255, 0, 0, 255));
            // Рисуем модель оружия (простая позиция)
            if (Raylib.IsModelReady(weaponModel))
            {
                Raylib.DrawModel(weaponModel, new System.Numerics.Vector3(1.0f, -0.5f, 1.0f), 1.0f, new Raylib_cs.Color(255, 255, 255, 255));
            }
            else
            {
                // Если модель не загрузилась, рисуем куб вместо неё
                Raylib.DrawCube(new System.Numerics.Vector3(1.0f, -0.5f, 1.0f), 1.0f, 1.0f, 1.0f, new Raylib_cs.Color(0, 0, 255, 255));
            }
            Raylib.EndMode3D();

            // Рисуем UI
            Raylib.DrawText("CS3 - WASD для движения, мышь для взгляда", 10, 10, 20, new Raylib_cs.Color(255, 255, 255, 255));
            Raylib.DrawText("ЛКМ для выстрела (звук)", 10, 40, 20, new Raylib_cs.Color(255, 255, 255, 255));
            Raylib.DrawFPS(10, 70);

            Raylib.EndDrawing();
        }

        // Очистка
        Raylib.UnloadSound(shootSound);
        Raylib.UnloadModel(weaponModel);
        Raylib.CloseAudioDevice();
        Raylib.CloseWindow();
    }
}