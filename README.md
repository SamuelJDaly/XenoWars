<h1>XenoWars</h1>
<p>Note: This readme is a work in progress...</p>
<p>
XenoWars is a simple turn based 4x game intended to provide an opportunity to develop my game development skills beyond Pong. While the challenges of this 
  style of game are numerous, I anticipate the greatest challenges to be designing a compelling computer adversary, and properly balancing the various economic systems.
</p>

<h2>Framework</h2>
<p>Primary Language: C++ 17</p>
<p>Other Languages: None yet...</p>

<p>Frameworks / Libraries:</p>
<ul>
  <li><a href=https://github.com/SFML/SFML/releases>SFML 3.0</a>: Windowing, Rendering, I/O, Events</li>
  <li><a href=https://github.com/ocornut/imgui/tree/v1.91.9>ImGUI 1.91</a>: GUI</li>
  <li><a href=https://github.com/SFML/imgui-sfml>Imgui-SFML</a>: ImGUI sfml backend</li>
</ul>

<h2>Game Systems</h2>
<p>Listed below are some of the game systems that I think are worth mentioning and the thought processes behind them.</p>
<h3>Engine</h3>
<p>The engine system is what our program entry point calls to start and manage the game. It's main responsibilites are to manage the
state of the game, load resources like textures and audio, and provide those resources to the active state. The engine provides the sturcture of
the gameloop in three functions; Poll for polled events like keyboard and mouse input, Update for things that happen on a per frame
basis, and Draw for rendering the various drawable components of the game.</p>

<h3>Game States</h3>
<p>The gamestate system is the meat of the game and is responsible for.../p>
<h3>Map</h3>
<h3>AI</h3>
<h3>Serialization</h3>
