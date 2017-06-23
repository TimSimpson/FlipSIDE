#include "CharacterSprite.hpp"


// SUPER DUPER DIRECT X 8 MODULE

Const FVF = D3DFVF_XYZRHW Or D3DFVF_TEX1 Or D3DFVF_DIFFUSE Or D3DFVF_SPECULAR


// IT'S TIME FOR DIRECT X ACTION!
Dim DX As DirectX8
Dim d3d As Direct3D8
Dim dev As Direct3DDevice8
Dim d3dx As D3DX8 // Direct 3d x 8 object (?)
Dim bgtexture As Direct3DTexture8

Dim bgSurface As Direct3DSurface8


Dim AnimationTexture(9) As Direct3DTexture8
Dim texWidth(9) As Integer
Dim texHeight(9) As Integer


Dim m_d3dpp As D3DPRESENT_PARAMETERS

// Background vertices
Dim bgverts(3) As Vertex

Dim windowedMode As Boolean

Dim levelPath As String

Dim m_d3dcaps As D3DCAPS8

// Don't know if I need this... oh wait I do
Dim m_d3ddm As D3DDISPLAYMODE


Private Declare Function GetTickCount Lib "kernel32" () As Long



Private Function StartUpDX(DX As DirectX8, d3d As Direct3D8, dev As Direct3DDevice8, ByVal hwnd As Long, Optional ByVal bWindowed As Boolean = True) As Long
// ALL YOUR CODE ARE BELONG TO MICROSOFT DONUTS!
Dim devtype As CONST_D3DDEVTYPE
Dim i As Long, lCount As Long, lErrNum As Long, format As Long
Dim bFoundMode As Boolean

On Local Error Resume Next
windowedMode = bWindowed // Store windowed mode that was passed in
If windowedMode = True Then Form1.MousePointer = 1 Else Form1.MousePointer = 99

// This initializes dx8
Set DX = New DirectX8

    If Err.Number Then

        // There were problems creating the dx object. Return the error number.
        StartUpDX = Err.Number
        Exit Function

    End If

Set d3d = DX.Direct3DCreate

    If Err.Number Then

        // There were problems creating the d3d object. Return the error number.
        StartUpDX = Err.Number
        Exit Function

    End If

// Attempts to CREATE HAL> Good morning dave.
devtype = D3DDEVTYPE_HAL

    // "Get the capabilities of the Direct3D device that we specify. In this case,
    // we'll be using the adapter default (the primiary card on the system)."
    //                            Donuts, 2000
    //            I couldn't have said it better myself.
    Call d3d.GetDeviceCaps(D3DADAPTER_DEFAULT, devtype, m_d3dcaps)

    // Donuts also told me this:
    // "Check for errors. If there is an error, the card more than likely doesn't support at least DX7,
    // so get the caps of the reference device instead."
    //           Donuts, you're so funny when you're drunk!
    If Err.Number Then

        Err.Clear  //  If theres an error, heeeeeeeey, relax guy
        devtype = D3DDEVTYPE_REF   // get whatever old hal recommended.  Stupid sleazy hal!
        Call d3d.GetDeviceCaps(D3DADAPTER_DEFAULT, devtype, m_d3dcaps)

        // If there is *still* an error, then the driver has problems. We'll
        // have to exit at this point, because there isn't anything else we can
        // do.
        If Err.Number Then
            StartUpDX = D3DERR_NOTAVAILABLE
            Exit Function
        End If

    End If

// Gets some info about the display mode.
        Call d3d.GetAdapterDisplayMode(D3DADAPTER_DEFAULT, m_d3ddm)
// Apparently, we are now filling the D3DPRESENT_PARAMETERS type.
With m_d3dpp

     If windowedMode Then
           // Make sure there are more than 8-bits per pixel.
           // What losers use eight bits per pixel?  Lame brains! Hywark! Hywark!
           If m_d3ddm.format = D3DFMT_P8 Or m_d3ddm.format = D3DFMT_A8P8 Then
           // Device is running an 8bit variation format of some kind
           MsgBox ("Make no in 8bit")
           StartUpDX = D3DERR_INVALIDDEVICE
           Exit Function
        Else
           // Device is more than 8-bits (thank JESUS). Set format variable to current display format so we can make a note of it.
           format = m_d3ddm.format
        End If

        // Donuts sayz "For Windowed mode, we just discard information instead of flipping it."
        // What the hell does that mean?
        .SwapEffect = D3DSWAPEFFECT_DISCARD

        // Set windowed mode to true
        .Windowed = 1

    Else  // Enough with the windowed mode crap, its full screen time baby!

        // Call a sub to find the first suitable full screen format
        lErrNum = FindMode(FULLSCREENWIDTH, FULLSCREENHEIGHT, format)

                    // If unable to find a suitable mode, the app will have to exit.
            If lErrNum Then
                MsgBox " Your computer sucks. ", vbCritical
                StartUpDX = D3DERR_INVALIDDEVICE
                Exit Function
            End If

            .SwapEffect = D3DSWAPEFFECT_FLIP

            .BackBufferWidth = SimulatedWidth
            .BackBufferHeight = SimulatedHeight

 End If

.BackBufferFormat = format
End With

// Now that all that craps done, its time to set the DEVICE OF DEATH!

//   Set dev = d3d.CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, m_d3dpp)
   Set dev = d3d.CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, m_d3dpp)

 // If the creation above failed, try to create a REF device instead.
    If Err.Number Then

       //  Err.Clear
//         Set dev = d3d.CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, m_d3dpp)
        Set dev = d3d.CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, m_d3dpp)

        If Err.Number Then
                    MsgBox ("Monkey Turd")
            // The app still hit an error. Both HAL and REF devices weren't created. The app will have to exit at this point.
            StartUpDX = Err.Number
            Exit Function

        End If
    End If

// Store dimensions
If windowedMode Then
  RealWidth = Form1.ScaleWidth
  RealHeight = Form1.ScaleHeight
Else
 RealWidth = FULLSCREENWIDTH
 RealHeight = FULLSCREENHEIGHT

End If

   If InitDevice(dev, hwnd) Then

        MsgBox "Unable to initialize the device"
        Unload Form1

    End If

End Function

Function FindMode(ByVal w As Long, ByVal h As Long, fmt As Long) As Long

// ALL YOUR CODE ARE BELONG TO DONUTS!

  Dim i  As Long, lCount As Long
    Dim d3ddm As D3DDISPLAYMODE
    Dim bFoundMode As Boolean

    i = 0

    // Get the number of adapter modes this adapter supports.
    lCount = d3d.GetAdapterModeCount(D3DADAPTER_DEFAULT) - 1

    // If we encounter an error, return an error code and exit the function.
    If Err.Number Then
        FindMode = D3DERR_INVALIDCALL
        Exit Function
    End If

    // Next, loop through all the display modes until we find one
    // that matches the parameters passed in.
    For i = 0 To lCount

        Call d3d.EnumAdapterModes(D3DADAPTER_DEFAULT, i, d3ddm)

        // Again, catch any unexpected errors.
        If Err.Number Then
            FindMode = Err.Number
            Exit Function
        End If

        // Check to see if this mode matches what is being sought.
        If d3ddm.Width = w And d3ddm.Height = h Then

            // Now see if this mode is either a 32bpp or 16bpp mode
            If d3ddm.format = D3DFMT_R8G8B8 Or _
                d3ddm.format = D3DFMT_R5G6B5 Then

                // We've found a suitable display. Set the flag
                // to reflect this, and exit. No need to look further.
                bFoundMode = True

                // Set the fmt to the format that was found.
                fmt = d3ddm.format

                Exit For
            End If
        End If
    Next

    If bFoundMode Then

        // Everything checked out OK
        Exit Function

    Else

        // Return an error
        FindMode = D3DERR_INVALIDDEVICE

    End If


End Function

Function InitDevice(dev As Direct3DDevice8, hwnd As Long) As Long
Static madeShapes As Boolean
Call makeshapes


With dev
    Call .SetVertexShader(FVF)

    Call .SetRenderState(D3DRS_LIGHTING, 0)

    Call .SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA)

    Call .SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)

End With

If Err.Number Then InitDevice = D3DERR_INVALIDCALL


End Function

Sub makeshapes()
Dim j As Integer
Static madeShapes As Boolean


// This makes shapes
With bgverts(0)
    .x = 0: .y = 480 //  RealHeight
    .tu = 0: .tv = 1
    .rhw = 1
    .color = normColor

End With
With bgverts(1)
    .x = 0: .y = 0
    .tu = 0: .tv = 0
    .rhw = 1
    .color = normColor
End With

With bgverts(2)
    .x = 640: .y = 480 //  RealWidth: .y = RealHeight
    .tu = 1: .tv = 1
    .rhw = 1
    .color = normColor
End With
With bgverts(3)
    .x = 640: .y = 0
    .tu = 1: .tv = 0
    .rhw = 1
    .color = normColor
End With

For j = 0 To spritesInUse
With Sprite(j)
    With .SpriteVerts(0)
      .x = Sprite(j).x
      .y = Sprite(j).y + Sprite(j).high
      .tu = 0: .tv = 0.5
      .rhw = 1
      .color = normColor

    End With
    With .SpriteVerts(1)
      .x = Sprite(j).x
      .y = Sprite(j).y
      .tu = 0: .tv = 0
      .rhw = 1
      .color = normColor
    End With
    With .SpriteVerts(2)
      .x = Sprite(j).x + Sprite(j).wide
      .y = Sprite(j).y + Sprite(j).high
      .tu = 0.5: .tv = 0.5
      .rhw = 1
      .color = normColor
    End With
    With .SpriteVerts(3)
      .x = Sprite(j).x + Sprite(j).wide
      .y = Sprite(j).y
      .tu = 0.5: .tv = 0
      .rhw = 1
      .color = normColor
    End With



End With
Next j

madeShapes = True
End Sub

// Moved to main.cpp

Sub loadTexture(which As Integer, fileName As String, howWide As Integer, howHigh As Integer)
 // On Local Error Resume Next
If which = -1 Then
 Set bgtexture = d3dx.CreateTextureFromFile(dev, FilePath + fileName)
 bgWidth = howWide: bgHeight = howHigh

End If
If which <> -1 Then

 Set AnimationTexture(which) = d3dx.CreateTextureFromFileEx( _
                                                   dev, _
                                                   FilePath + fileName, _
                                                   D3DX_DEFAULT, _
                                                   D3DX_DEFAULT, _
                                                   D3DX_DEFAULT, _
                                                   0, _
                                                   D3DFMT_UNKNOWN, _
                                                   D3DPOOL_MANAGED, _
                                                   D3DX_FILTER_POINT, _
                                                   D3DX_FILTER_POINT, _
                                                   maskColor, _
                                                   ByVal 0, _
                                                   ByVal 0 _
                                                   )
texWidth(which) = howWide: texHeight(which) = howHigh
End If

End Sub
Function ResetDevice() As Long
On Local Error Resume Next
Call dev.Reset(m_d3dpp)

If Err.Number Then
  ResetDevice = Err.Number
  Exit Function

End If

Call InitDevice(dev, Form1.hwnd)

End Function

Sub level1()

destroyEverything

Call NowLoading
Dim j As Integer

Call initPlayers
For j = 0 To 2
lives(j) = 3
continues = 1
Next j


CameraWidth = 640: CameraHeight = 480
StopSound 0
loadLevel "Level1.cap"
cameraStopX = 1010
cameraStopY = 905 + 480
Gravity = 20


CameraX = 0: CameraY = 0
view.loadTexture(-1, "lv1bg.bmp", 10, 10);
// Call loadTexture(0, "smile.bmp", 255, 255)

// Call loadTexture(4, "level1a.bmp", 490, 209)
// Call loadTexture(5, "lv1bgtw.bmp", 308, 341)
// Call loadTexture(6, "goomba.bmp", 240, 240)
view.loadTexture(9, "level1cinema.bmp", 400, 400);



Sprite(0).x = 50
Sprite(0).y = 220
Sprite(10).x = 50
Sprite(10).y = 220
Sprite(20).x = 50
Sprite(20).y = 220

For j = 0 To spritesInUse
Call initSprites(j)
Next j

Sprite(33).name = "cinema": Sprite(33).zOrder = -149
Sprite(32).name = "cinema": Sprite(32).zOrder = -149
Sprite(31).name = "cinema": Sprite(31).zOrder = -149
Sprite(34).name = "cinema": Sprite(34).zOrder = -149
Sprite(30).zOrder = -149
Call loadAnimation(33, "level1cinema.ani")
Call loadAnimation(32, "level1cinema.ani")
Call loadAnimation(31, "level1cinema.ani")
Call loadAnimation(34, "level1cinema.ani")
Sprite(30).name = "StageWhat"

cinemaMax = 2
cinemaCounter = 0
setCinema 0, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1a1.wav", 4.49
setCinema 1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1a2.wav", 1.87





Call findOrder



LoadSound 0, "fireball.wav", "fireball"
LoadSound 1, "death.wav", "DavidDeath"
LoadSound 2, "hurt.wav", "DavidHurt"
LoadSound 3, "goomba.wav", "dying explosion"
LoadSound 4, "spring.wav", "spring"
LoadSound 5, "goombaouch.wav", "Goomba Ouch"
LoadSound 6, "KerboDie.wav", "Kerbose Die"
LoadSound 7, "KerbHurt.wav", "Kerbose Ouch"
LoadSound 8, "Setbomb.wav", "set bomb"
LoadSound 9, "Bomb Explode.wav", "bomb explode"
LoadSound 10, "NickyDeath.wav", "NickyDeath"
LoadSound 11, "NickyHurt.wav", "NickyHurt"
LoadSound 12, "putulohurt.wav", "putulohurt"
LoadSound 13, "putulodie.wav", "putulodie"
playBGM "Level1Opening.wav"

Exit Sub





Gravity = 20
Sprite(0).x = 640
Sprite(0).y = 480
Sprite(0).name = "Thomas"
For j = 1 To 9
Sprite(j).x = 60
Sprite(j).y = 70
Sprite(j).name = "fireball"
Next j
Sprite(81).x = 400
Sprite(81).y = 250
Sprite(81).name = "Deadly Rat"
// Call initSprites(31)
Sprite(82).x = 440
Sprite(82).y = 150
Sprite(82).z = 0

Sprite(82).name = "goomba"
Sprite(83).x = 350
Sprite(83).y = 150
Sprite(83).name = "goomba"

Sprite(84).x = 240
Sprite(84).y = 300
Sprite(84).name = "pigeon"


Sprite(50).x = 50
Sprite(50).y = 50
Sprite(50).name = "treewalk"
Sprite(51).name = "treewalk"
Sprite(51).x = 50
Sprite(51).y = 350
Sprite(52).name = "treewalk"
Sprite(52).x = 50
Sprite(52).y = 650
Sprite(53).name = "treewalk"
Sprite(53).x = 50
Sprite(53).y = 950
Sprite(54).x = 0
Sprite(54).y = 0
Sprite(54).wide = 50
Sprite(54).high = 1000
Sprite(54).name = "grass"

Sprite(55).x = 640
Sprite(55).y = 300
Sprite(55).name = "pigeon"
Sprite(56).x = 400
Sprite(56).y = 480
Sprite(56).name = "pigeon"
Sprite(87).x = 400
Sprite(87).y = 480
Sprite(87).name = "goomba"



Call findOrder



End Sub

Sub gameOver()
playBGM ""


screen$ = "gameOver"
destroyEverything
view.loadTexture(0, "GameOver.bmp", 320, 287)
With Sprite(0)
.srcx = 1: .srcy = 1: .srcx2 = 320: .srcy2 = 240
.x = 0: .y = 0: .wide = 640: .high = 480: .visible = True: .trueVisible = 1
.name = "GameOverCloudBg": .texture = 0: .color = normColor
End With
With Sprite(1)
.srcx = 1: .srcy = 243: .srcx2 = 320: .srcy2 = 287
.x = 0: .y = 180: .wide = 640: .high = 94: .visible = True: .trueVisible = 1
.name = "GameOverCloudTitle": .texture = 0
.miscTime = clock + 4
End With
playWave "gameover.wav"
Call findOrder
End Sub

// DestroyedEverything- moved to Game.cpp

Sub titleScreen()

Dim j As Integer

For j = 0 To 2
lives(j) = 3
continues = 2
Next j

// playBGM ""


// Call loadTexture(-1, "bg.bmp", 300, 300)

destroyEverything
playWave "Opening.wav"

view.loadTexture(0, "title2.bmp", 285, 427);
view.loadTexture(1, "title1.bmp", 440, 427);
// loadTexture 2, "goomba.bmp", 240, 240
// Sprite(11).name = "goomba"
// Call initSprites(11)
With Sprite(0)
.srcx = 5: .srcy = 137: .srcx2 = 324: .srcy2 = 318
.x = 1: .y = 1: .wide = 640: .high = 480: .visible = False
.name = "TitleBg1": .texture = 1
.miscTime = clock + 20
End With
With Sprite(1)
.srcx = 1: .srcy = 3: .srcx2 = 196: .srcy2 = 107
.x = 0: .y = 180: .wide = 193: .high = 106: .visible = False
.name = "Title1": .miscTime = clock + 2
End With
With Sprite(2)
.srcx = 2: .srcy = 111: .srcx2 = 279: .srcy2 = 230
.x = 0: .y = 174: .wide = 277: .high = 119: .visible = False
.name = "Title1": .miscTime = clock + 6
End With
With Sprite(3)
.srcx = 1: .srcy = 233: .srcx2 = 224: .srcy2 = 363
.x = 0: .y = 168: .wide = 232: .high = 130: .visible = False
.name = "Title1": .miscTime = clock + 10
End With
With Sprite(4)
.srcx = 1: .srcy = 366: .srcx2 = 198: .srcy2 = 424
.x = 0: .y = 228: .wide = 197: .high = 58: .visible = False
.name = "Title1": .miscTime = clock + 14
End With
With Sprite(5)
.srcx = 9: .srcy = 6: .srcx2 = 348: .srcy2 = 81
.x = 1: .y = -240: .wide = 640: .high = 960: .visible = False
.name = "Title2": .miscTime = clock + 20: .texture = 1
End With
With Sprite(6)
.srcx = 7: .srcy = 91: .srcx2 = 437: .srcy2 = 128
.x = -320: .y = 140 + 213: .wide = 1280: .high = 110: .visible = False
.name = "Title3": .miscTime = clock + 20: .texture = 1
End With

Call findOrder
End Sub


Sub switchWindowedMode()
// ***********************************************************************
    //
    //  This sub switches the current display mode between windowed/fullscreen.
    //  If it runs into an error, it just exits, leaving the display mode in
    //  its current state.
    //
    //  Parameters:
    //            None.
    //
    // ***********************************************************************

    Dim d3dppEmpty As D3DPRESENT_PARAMETERS
    Dim format As Long
    Dim lErrNum As Long

    On Local Error Resume Next

    If windowedMode Then

        // Grab a valid format for this device. If a format
        // for the requested resolution wasn't found, exit the sub.
        If FindMode(FULLSCREENWIDTH, FULLSCREENHEIGHT, format) <> 0 Then Exit Sub

        // Store the current window mode format
        Call d3d.GetAdapterDisplayMode(D3DADAPTER_DEFAULT, m_d3ddm)

        // The app is running windowed currently, switch to fullscreen.
        windowedMode = False

        // Set the present parameters for running full screen
        m_d3dpp = d3dppEmpty

        With m_d3dpp
            .SwapEffect = D3DSWAPEFFECT_FLIP
            .BackBufferFormat = format
            .BackBufferWidth = FULLSCREENWIDTH
            .BackBufferHeight = FULLSCREENHEIGHT
            .Windowed = 0
        End With

        // Store the client dimensions
        SimulatedWidth = FULLSCREENWIDTH
        SimulatedHeight = FULLSCREENHEIGHT

        // Reset the device to the new mode
        lErrNum = ResetDevice

        // If there is an error resetting the device,
        // just exit the sub.
        If lErrNum Then
            // Store the client dimensions
            SimulatedWidth = Form1.ScaleWidth
            SimulatedHeight = Form1.ScaleHeight
            windowedMode = True
            Exit Sub
        End If

    Else

        // Set the present params to reflect windowed operation.
        m_d3dpp = d3dppEmpty

        With m_d3dpp
            .SwapEffect = D3DSWAPEFFECT_DISCARD
            .BackBufferFormat = m_d3ddm.format
            .Windowed = 1
        End With

        // Reset the device to the new mode
        lErrNum = ResetDevice

        // If there is an error, just exit the sub
        If lErrNum Then
            windowedMode = False
            Exit Sub
        End If

        // Now get the device ready again
        Call InitDevice(dev, Form1.hwnd)

        // Resize the form to the size it was previous to going fullscreen.
        Form1.Width = SimulatedWidth * 12.5 // * 'screen.TwipsPerPixelX
       Form1.Height = SimulatedHeight * 12.5 // * screen.TwipsPerPixelY

        // The app is now running windowed
        windowedMode = True

        // Store the client dimensions
        SimulatedWidth = Form1.ScaleWidth
        SimulatedHeight = Form1.ScaleHeight

        // Resize the window to the old size now.
        // Call Form1.Form_Resize

    End If

If windowedMode = True Then Form1.MousePointer = 1 Else Form1.MousePointer = 99


End Sub


Sub debugLevel()
Call NowLoading
StopSound 0
destroyEverything

Dim j As Integer

CameraX = 0: CameraY = 0
view.loadTexture(-1, "lv1bg.bmp", 10, 10);
view.loadTexture(0, "smile.bmp", 255, 255);
view.loadTexture(1, "Flip1.bmp", 254, 254);
view.loadTexture(4, "goomba.bmp", 490, 209);
view.loadTexture(5, "lv1bgtw.bmp", 308, 341);
view.loadTexture(6, "grass.bmp", 17, 13);
gotFocus = 0


Gravity = 20
Sprite(0).x = 50
Sprite(0).y = 300
Sprite(0).name = "Thomas"
For j = 1 To 9
Sprite(j).x = 60
Sprite(j).y = 70
Sprite(j).name = "fireball"
Next j



Sprite(82).x = 440
Sprite(82).y = 150
Sprite(82).z = 0

Sprite(82).name = "goomba"
Sprite(83).x = 350
Sprite(83).y = 150
Sprite(83).name = "goomba"

For j = 0 To spritesInUse
Call initSprites(j)
Next j
Call findOrder
End Sub








End Sub


Function unstretch(which As Integer)
With Sprite(which)
If .frame = 0 Then
.wide = .srcx2 - .srcx
.high = .srcy2 = .srcy
Else
.wide = .Aframe(.frame).x2 - .Aframe(.frame).x
.high = .Aframe(.frame).y2 - .Aframe(.frame).y
End If

End With
End Function

Function makeJump(which As Integer)
If Sprite(which).z = 0 Then Sprite(which).multiJump = 0
If Sprite(which).multiJump >= Sprite(which).maxJump Then Exit Function
Sprite(which).multiJump = Sprite(which).multiJump + 1
Sprite(which).jumpStart = Sprite(which).z
Sprite(which).jumpTime = cscript(lock
End Function


Function seeker(who As Integer)
With Sprite(who)
If .x < .seekx Then .x = .x + (.mph * sFactor)
If .x > .seekx Then .x = .x - (.mph * sFactor)
If .y < .seeky Then .y = .y + (.mph * sFactor)
If .y > .seeky Then .y = .y - (.mph * sFactor)
End With
End Function


Function shoot(who As Integer, what As String, wherex, wherey)

Dim opera As Integer

For opera = (who + 1) To spritesInUse
If Sprite(opera).name = "" Or Sprite(opera).name = "empty" Or Sprite(opera).name = "dead" Then
// killS opera
Sprite(opera).name = what
Exit For
End If



Next opera
If opera >= 95 Then Exit Function

Sprite(opera).trueVisible = 0
Sprite(opera).visible = True
Sprite(opera).flickOn = False
Sprite(opera).texture = Sprite(who).texture
Sprite(opera).wide = Sprite(who).wide
Sprite(opera).high = Sprite(who).high

Call initSprites(opera)
Sprite(opera).zOrder = -1
Sprite(opera).x = Sprite(who).x
Sprite(opera).y = Sprite(who).y
Sprite(opera).z = Sprite(who).z
Sprite(opera).seekx = wherex
Sprite(opera).seeky = wherey
Call findOrder

End Function

Function killLimit(jex As Integer)
// Rem- Kills unruly sprites who are out of bounds
With Sprite(jex)
If .x > cameraStopX Or .x < -10 Then killS jex

If .y > cameraStopX Or .y < -10 Then killS jex
End With
End Function



Function loadframe(jex As Integer, whichframe As Integer, wx1 As Integer, wy1 As Integer, wx2 As Integer, wy2 As Integer)
With Sprite(jex).Aframe(whichframe)
.x = wx1
.y = wy1
.x2 = wx2
.y2 = wy2
End With



End Function


Private Function getErrorCode(errNum)
If errNum = D3D_OK Then MsgBox ("It's All good.")
If errNum = d3DERR_CANNOTTATTRSORT Then MsgBox ("D3DERR_CANNOTATTRSORT")
If errNum = D3DERR_CONFLICTINGRENDERSTATE Then MsgBox ("D3DERR_CONFLICTINGRENDERSTATE")
If errNum = D3DERR_CONFLICTINGTEXTUREFILTER Then MsgBox ("D3DERR_CONFLICTINGTEXTUREFILTER")
If errNum = D3DERR_CONFLICTINGTEXTUREPALETTE Then MsgBox ("D3DERR_CONFLICTINGTEXTUREPALETTE ")
If errNum = D3DERR_DEVICELOST Then MsgBox ("D3DERR_DEVICELOST ")
If errNum = D3DERR_DEVICENOTRESET Then MsgBox ("D3DERR_DEVICENOTRESET ")
If errNum = D3DERR_DRIVERINTERNALERROR Then MsgBox ("D3DERR_DRIVERINTERNALERROR ")
If errNum = D3DERR_INVALIDCALL Then MsgBox ("D3DERR_INVALIDCALL ")
If errNum = D3DERR_INVALIDDEVICE Then MsgBox ("D3DERR_INVALIDDEVICE ")
If errNum = D3DERR_MOREDATA Then MsgBox ("D3DERR_MOREDATA ")
If errNum = D3DERR_NOTAVAILABLE Then MsgBox ("D3DERR_NOTAVAILABLE ")
If errNum = D3DERR_NOTFOUND Then MsgBox ("D3DERR_NOTFOUND ")
If errNum = D3DERR_OUTOFVIDEOMEMORY Then MsgBox ("D3DERR_OUTOFVIDEOMEMORY ")
If errNum = D3DERR_TOOMANYOPERATIONS Then MsgBox ("D3DERR_TOOMANYOPERATIONS ")
If errNum = D3DERR_UNSUPPORTEDALPHAARG Then MsgBox ("D3DERR_UNSUPPORTEDALPHAARG ")
If errNum = D3DERR_UNSUPPORTEDALPHAOPERATION Then MsgBox ("D3DERR_UNSUPPORTEDALPHAOPERATION ")
If errNum = D3DERR_UNSUPPORTEDCOLORARG Then MsgBox ("D3DERR_UNSUPPORTEDCOLORARG ")
If errNum = D3DERR_UNSUPPORTEDCOLOROPERATION Then MsgBox ("D3DERR_UNSUPPORTEDCOLOROPERATION ")
If errNum = D3DERR_UNSUPPORTEDFACTORVALUE Then MsgBox ("D3DERR_UNSUPPORTEDFACTORVALUE ")
If errNum = D3DERR_UNSUPPORTEDTEXTUREFILTER Then MsgBox ("D3DERR_UNSUPPORTEDTEXTUREFILTER ")
If errNum = D3DERR_WRONGTEXTUREFORMAT Then MsgBox ("D3DERR_WRONGTEXTUREFORMAT ")
If errNum = E_FAIL Then MsgBox ("E_FAIL ")
If errNum = E_INVALIDARG Then MsgBox ("E_INVALIDARG ")
If errNum = E_INVALIDCALL Then MsgBox ("E_INVALIDCALL ")
If errNum = E_OUTOFMEMORY Then MsgBox ("E_OUTOFMEMORY ")

End Function
