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
Function endgame()
STOPGAME = True

End Function
Function hitdetection(num1 As Integer, num2 As Integer, Optional whatKind As Boolean) As Long
hitdetection = 0

Dim k As Integer
Dim j As Integer
Dim con1%
Dim con2%
Dim con3%
Dim fapple%
Dim crapapple%

Dim left%
Dim right%
Dim up%
Dim down%
// Rem-checks to see if they hit from left or right
// For j = 0 To BIGO
// For k = j To BIGO
con1% = 0: con2% = 0: con3% = 0
// If code(j) = 0 Or code(k) = 0 Then GoTo screwthis3
// If k = j Then GoTo screwthis
// lower numbers get screwed first
con1% = 0
If (Sprite(num1).x + Sprite(num1).wide) >= Sprite(num2).x And Sprite(num1).x < Sprite(num2).x Then con1% = 1: left% = 1
// If Sprite(num1).x = Sprite(num2).x Then con1% = 1: right% = 1
// If (Sprite(num1).x + Sprite(num1).wide) = Sprite(num2).x Then con1% = 1: left% = 1
// If Sprite(num1).x = Sprite(num2).x + Sprite(num2).wide Then con1% = 1: right% = 1
If (Sprite(num2).x + Sprite(num2).wide) >= Sprite(num1).x And Sprite(num2).x < Sprite(num1).x Then con1% = 1: right% = 1

// If con1% = 1 Then Picture1.Line (sprite(num1).x, sprite(num1).y)-(sprite(num1).x + sprite(num1).wide, sprite(num1).y + sprite(num1).high), QBColor(4), BF 'Beep: Label2.Caption = "From Right of " + Str$(j)


screwthis:


// Rem- Checks to see if they ever collide from the top to bottom (Y)
con2% = 0
// If k = j Then GoTo screwthis2

If (Sprite(num1).y + Sprite(num1).high) >= Sprite(num2).y And Sprite(num1).y < Sprite(num2).y Then con2% = 1: up% = 1
// If Sprite(num1).y = Sprite(num2).y Then con2% = 1: down% = 1
// If (Sprite(num1).y + Sprite(num1).high) = Sprite(num2).y Then con2% = 1: up% = 1
// Rem- Added as of 11/27/00
// If Sprite(num1).y = Sprite(num2).y + Sprite(num2).high Then con2% = 1: down% = 1
If (Sprite(num2).y + Sprite(num2).high) >= Sprite(num1).y And Sprite(num2).y < Sprite(num1).y Then con2% = 1: down% = 1


// If con2% = 1 Then Picture1.Line (sprite(num1).x, sprite(num1).y)-(sprite(num1).x + sprite(num1).wide, sprite(num1).y + sprite(num1).high), QBColor(4), BF 'Beep: Label2.Caption = Label2.Caption + "From Bottom of " + Str$(j)

screwthis2:
If Sprite(num1).kind = 5 Or Sprite(num2).kind = 5 Then con3% = 1: GoTo screwthis3


// Rem- The THIRD dimension, Z!

// If k = j Then GoTo screwthis3

If (Sprite(num1).z + (Sprite(num1).length * 1.5)) >= Sprite(num2).z And Sprite(num1).z < Sprite(num2).z Then con3% = 1
If Sprite(num1).z = Sprite(num2).z Then con3% = 1
// If (Sprite(num1).z + Sprite(num1).length) = Sprite(num2).z Then con3% = 1
// Rem- Added as of 11/27/00
// If Sprite(num1).z = Sprite(num2).z + Sprite(num2).length Then con3% = 1
If (Sprite(num2).z + Sprite(num2).length * 1.5) >= Sprite(num1).z And Sprite(num2).z < Sprite(num1).z Then con3% = 1


// If con3% = 1 Then Print "": Rem-Beep: 'Label2.Caption = "From Top of " + Str$(j)

screwthis3:
// 'If con3% = 1 And con2% = 1 And con1% = 1 Then
// Picture1.Line (Sprite(num1).x, Sprite(num1).y)-(Sprite(num1).x + Sprite(num1).wide, Sprite(num1).y + Sprite(num1).high), QBColor(4), BF 'Beep: Label2.Caption = Label2.Caption + "From Bottom of " + Str$(j)
// Picture1.Line (Sprite(num2).x, Sprite(num2).y)-(Sprite(num2).x + Sprite(num2).wide, Sprite(num2).y + Sprite(num2).high), QBColor(4), BF
// If k = fapple% And j = crapapple% Then GoTo skipitcausetheyalreadyweredetected
// 'hitdetection = 1: Let fapple% = j: crapapple% = k
// Call colide(k, j)
// 'skipitcausetheyalreadyweredetected:
// 'End If

// Next k
// Next j


// If con1% = 1 Then hitdetection = 2
// If con2% = 1 Then hitdetection = 3
// If con3% = 1 Then hitdetection = 4
// If con1% = 1 And con2% = 1 Then hitdetection = 5
// If con2% = 1 And con3% = 1 Then hitdetection = 6
// If con1% = 1 And con3% = 1 Then hitdetection = 7
If con1% = 1 And con2% = 1 And con3% = 1 Then hitdetection = 1
If whatKind = True Then
If left% = 1 Then hitdetection = 2
If right% = 1 Then hitdetection = 3
If up% = 1 Then hitdetection = 4
If down% = 1 Then hitdetection = 5
If left% = 1 And up% = 1 Then hitdetection = 6
If left% = 1 And down% = 1 Then hitdetection = 7
If right% = 1 And up% = 1 Then hitdetection = 8
If right% = 1 And down% = 1 Then hitdetection = 9

End If


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
loadLevel "level1.cap"
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
Sub initSprites(which As Integer)
// this makes all the sprites do their thing based on their name
Dim trueorg As Integer
With Sprite(which)
// .color = normColor
.mph = 1
If .name = "Thomas" Then
.zOrder = -99
.soundFile = "DavidHurt"
.wide = 40
.high = 50
// .texture = 1
.visible = True
.length = 20
.z = 0
.jumpStrength = 75
.kind = 1
.frame = 5: .dir = "u"
.invTime = 2
.speed = 0
.deathType = "Death of David"
.hp = 4
// .mode = "truck"
.mover = True
.maxJump = 1
End If

If .name = "Nick" Then
.zOrder = -99
.soundFile = "nickhurt"
.wide = 40
.high = 50
// .texture = 1
.visible = True
.length = 20
.z = 0
.jumpStrength = 75
.kind = 1
.frame = 5: .dir = "u"
.invTime = 2
.speed = 0
.deathType = "Death of Nick"
.hp = 4
// .mode = "truck"
.mover = True
.maxJump = 1
End If


If .name = "Nicky" Then
.zOrder = -99
.soundFile = "NickyHurt"
.wide = 26
.high = 30
// .texture = 1
.visible = True
.length = 20
.z = 0
.jumpStrength = 50
.kind = 1
.frame = 5: .dir = "u"
.invTime = 2
.speed = 0
.deathType = "Death of Nicky"
.hp = 4
// .mode = "truck"
.mover = True
.maxJump = 2
End If



If .name = "Death of David" Then
// StopSound which
// LoadSound which, "Death.wav"
PlaySound "DavidDeath"
.seekx = .wide * 10
.mph = 2
.kind = 0
.frame = 17
playWave "death.wav"
End If
If .name = "Death of Nicky" Then
.srcx = 1: .srcy = 46
.srcx2 = 14: .srcy2 = 60
.name = "Death of David"
PlaySound "NickyDeath"
.seekx = .wide * 10
.mph = 2
.kind = 0
End If
If .name = "Death of Nick" Then
.srcx = 1: .srcy = 46
.srcx2 = 14: .srcy2 = 60
.name = "Death of David"
PlaySound "nickdeath"
.seekx = .wide * 10
.mph = 2
.kind = 0

End If




If .name = "tdigger" Then
loadAnimation which, "tdigger.ani"
.hp = 1
End If

If .name = "clouds" Then
loadframe which, 1, .srcx, .srcy, .srcx2, .srcy2
End If


If .name = "paulrun" Then
loadAnimation which, "paulrun.ani"
.hp = 1
.kind = 7
.deathType = "gotmilkbs"
.invTime = 1
.hp = 2
.soundFile = "Paul Ouch"
.length = 20
End If


If .name = "paulbullet" Then
loadAnimation which, "paulbullet.ani"
.hp = 999
.kind = 2
.deathType = "Kerbose"
.invTime = 0
.wide = 10
.high = 10
.name = "bullet"
.mph = 2
End If


If .name = "bs death" Then
.visible = True
.mover = False: .kind = 0
.frame = 3
.miscTime = clock + 3
PlaySound "stick die"
.name = "Kerbose Death"
End If


If .name = "harharhar" Then
.flickerTime = clock + 2
playWave "harharhar.wav"
End If

If .name = "expand" Then
playWave "whawhee.wav"
.reverse = False
End If

If .name = "cinema" Then
.visible = False
.frame = 8
unstretch which
.mode = ""
End If



If .name = "Dying Explosion" Then
// StopSound which
// LoadSound which, "goomba.wav"
.flickerTime = clock + 10
.seekx = .wide * 3
.mph = 2
.kind = 0
// PlaySound which
PlaySound "dying explosion"
End If

If .name = "Kirbose" Or .name = "Kerbose" Then
.mover = True
.name = "Kerbose"
.wide = 21
.high = 19
.visible = True
.frame = 1
.kind = 7
.length = 10
.deathType = "Kerbose Death"
.invTime = 1
.hp = 3
.soundFile = "Kerbose Ouch"
.jumpStrength = 25
trueorg = Int(Rnd * 2) + 1
If trueorg = 1 Then
loadAnimation which, "kerbose.ani"
Else
loadAnimation which, "putulo.ani"
.soundFile = "putulohurt"
.deathType = "putuloDeath"
End If
End If

If .name = "gotmilkbs" Then
PlaySound "Paul Shrink"
.name = "bluestick"
End If

If .name = "greenspring" Then
loadAnimation which, "greenspring.ani"
.frame = 1
.mode = "off"
.hp = 1
.deathType = "greenspring"
.kind = 6
.jumpM = 1.5
.length = 10
.wide = .wide * 2
.high = .high * 2
End If


If .name = "bluestick" Then
.hp = 1
.kind = 7
.wide = 10
.high = 17
.length = 10
loadframe which, 1, 1, 173, 10, 190
loadframe which, 2, 13, 173, 23, 190
loadframe which, 3, 23, 174, 33, 190
.deathType = "bs death"
.soundFile = "Stick Ouch"
// .name = "Kerbose"
PlaySound "Stick Awaken"
.frame = 1
unstretch which
End If

If .name = "Kerbose Death" Then
.visible = True
// .name = "Dying Explosion"
// .flickerTime = clock + 10
// .seekx = .wide * 3
// .mph = 2
// .kind = 0

.mover = False: .kind = 0
.frame = 3
// srcx = 114: .srcy = 2
// .srcx2 = 134: .srcy2 = 19
.miscTime = clock + 3
PlaySound "Kerbose Die"
End If

If .name = "putuloDeath" Then
.visible = True
.mover = False: .kind = 0
.frame = 3
.miscTime = clock + 3
PlaySound "putulodie"
.name = "Kerbose Death"
End If

If .name = "fireball" Then
.wide = 40
.high = 50
.visible = False
.texture = 1
.frame = 1
.name = ""
.zOrder = -99
End If



If .name = "Deadly Rat" Then
.wide = 50
.high = 50
.texture = 0
.name = "Deadly Rat"
.hp = 5
.visible = True
.kind = 2
.deathType = "Dying Explosion"
.invTime = 1
End If

If .name = "falling" Then
.kind = 0
// .high = -30 '-1 * .high
End If

If .name = "pigeon" Then
.visible = True
.seekx = .x
.seeky = .y
.wide = 40
.high = 30
.hp = 1
.z = 80
.length = 30
.frame = 1
.deathType = "falling"
.kind = 7
End If

If .name = "pigeonbomber" Then
loadAnimation which, "pigeon.ani"
.visible = True
.seekx = -10
.seeky = .y
.wide = 40
.high = 30
.hp = 1
.z = 80
.length = 30
.frame = 1
.deathType = "falling"
.kind = 7
End If

If .name = "goomba" Then
.seekx = .x
.seeky = .y
.wide = 80
.high = 80
.length = 40
.texture = 6
.frame = 1
.mph = 1
.hp = 5
.speed = 0.25
.visible = True
.kind = 4
.invTime = 1
.deathType = "Dying Explosion"
.soundFile = "Goomba Ouch"
// LoadSound which, .soundFile
.mover = True
End If

If .name = "bullet" Then
.kind = 8
End If


// If .soundFile <> "" Then LoadSound which, .soundFile


End With
End Sub
Sub checkHit(j As Integer, k As Integer)
Dim redo As Boolean
Dim HolderJ As Integer
redo = False
redo2:
// If k = 1 And j = 31 Then Stop

// Rem-KIND DEFINITIONS
// 1 is player
// 2 is enemy
// 3 is fireball
// 4 is goomba thing
// 5 is unmoveable
// 6 is trampoline

// to do
// 7 is a enemy killable by jumping on it
// 8 is bullet by the enemy (can pass through 5's)

// Player hits an enemy
If Sprite(j).kind = 1 And (Sprite(k).kind = 2 Or Sprite(k).kind = 8) Then
If Sprite(j).flickerTime < clock Then
Sprite(j).hp = Sprite(j).hp - 1: PlaySound Sprite(j).soundFile
GoSub hurtj
If Sprite(j).hp <= 0 Then Sprite(j).name = Sprite(j).deathType: initSprites (j)
End If
End If

// Enemy hit by a fireball
If (Sprite(j).kind = 2 Or Sprite(j).kind = 7) And Sprite(k).kind = 3 Then

If Sprite(j).flickerTime < clock Then
Sprite(j).hp = Sprite(j).hp - 1
GoSub hurtj
If Sprite(j).hp <= 0 Then Sprite(j).name = Sprite(j).deathType: initSprites (j)
End If
If Sprite(k).name = "fireball" Then If slicer((Sprite(k).parent) / 10) = False Then killS k
End If

// Player steps on a goomba type thing
If Sprite(j).kind = 1 And (Sprite(k).kind = 4 Or Sprite(k).kind = 7) Then
If Sprite(j).z > Sprite(k).length And Sprite(j).lastJump > Sprite(j).z Then
Sprite(j).jumpStart = Sprite(j).z  // sends thing up into the air
Sprite(j).jumpTime = clock // sends thing up into the air, even if goomba is flashing
If Sprite(k).flickerTime < clock Then
Sprite(k).hp = Sprite(k).hp - 1
GoSub hurtK
PlaySound "spring"
Sprite(j).jumpM = Sprite(k).jumpM
If Sprite(k).hp <= 0 Then Sprite(k).name = Sprite(k).deathType: initSprites (k)
End If
End If
End If

// Player illigetimately touches goomba.
If Sprite(j).kind = 1 And (Sprite(k).kind = 4 Or Sprite(k).kind = 7) Then
If Sprite(j).flickerTime < clock Then
If Sprite(j).z < Sprite(k).length Or Sprite(j).lastJump < Sprite(j).z Then
Sprite(j).hp = Sprite(j).hp - 1
GoSub hurtj
If Sprite(j).hp <= 0 Then Sprite(j).name = Sprite(j).deathType: initSprites (j)
End If
End If
End If


// Player jumps on bouncy object
If Sprite(k).kind = 6 Then

If Sprite(j).kind = 3 Or Sprite(j).kind = 8 Then GoTo overalready

// BOUNCE TIME!
If Sprite(j).z > Sprite(k).length And Sprite(j).lastJump > Sprite(j).z Then
Sprite(j).jumpStart = Sprite(j).z  // sends thing up into the air
Sprite(j).jumpTime = clock // sends thing up into the air, even if goomba is flashing
PlaySound "spring"
Sprite(k).mode = "bounce": Sprite(k).miscTime = clock + 1: Sprite(j).jumpM = Sprite(k).jumpM
End If
// OH CRAP! I NO BOUNCE
If Sprite(j).z < Sprite(k).length Or Sprite(j).lastJump < Sprite(j).z Then
If Sprite(j).kind = 5 Or Sprite(j).kind = 3 Or Sprite(j).kind = 6 Then GoTo britneyDog2
// If Sprite(k).z > (Sprite(j).length - (Sprite(j).length * 0.75)) Then GoTo britneyDog2
poddle2:
// HolderJ = hitdetection(j, k, True)
// If HolderJ = 2 Or HolderJ = 6 Or HolderJ = 7 Then
// Sprite(j).x = Sprite(j).x - Sprite(j).mph
// End If
// If HolderJ = 3 Or HolderJ = 8 Or HolderJ = 9 Then
// Sprite(j).x = Sprite(j).x + Sprite(j).mph
// End If
// If HolderJ = 4 Or HolderJ = 6 Or HolderJ = 8 Then
// Sprite(j).y = Sprite(j).y - Sprite(j).mph
// End If
// If HolderJ = 5 Or HolderJ = 7 Or HolderJ = 9 Then
// Sprite(j).y = Sprite(j).y + Sprite(j).mph
// End If
// If hitdetection(k, j) = 1 Then GoTo poddle2 '
Sprite(j).x = Sprite(j).lastX
Sprite(j).y = Sprite(j).lastY
britneyDog2:

End If
overalready:
End If



If Sprite(j).kind = 5 Then
If Sprite(k).kind = 5 Or Sprite(k).kind = 3 Or Sprite(k).kind = 6 Or Sprite(k).kind = 8 Then GoTo britneyDog
poddle:
// HolderJ = hitdetection(k, j, True)
// If HolderJ = 2 Or HolderJ = 6 Or HolderJ = 7 Then
// Sprite(k).x = Sprite(k).x - Sprite(k).mph
// End If
// If HolderJ = 3 Or HolderJ = 8 Or HolderJ = 9 Then
// Sprite(k).x = Sprite(k).x + Sprite(k).mph
// End If
// If HolderJ = 4 Or HolderJ = 6 Or HolderJ = 8 Then
// Sprite(k).y = Sprite(k).y - Sprite(k).mph
// End If
// If HolderJ = 5 Or HolderJ = 7 Or HolderJ = 9 Then
// Sprite(k).y = Sprite(k).y + Sprite(k).mph
// End If
// If hitdetection(k, j) = 1 Then GoTo poddle
Sprite(k).x = Sprite(k).lastX
Sprite(k).y = Sprite(k).lastY
britneyDog:
End If

If redo = False Then redo = True: HolderJ = j: j = k: k = HolderJ: GoTo redo2
If redo = True Then HolderJ = j: j = k: k = HolderJ

Exit Sub

// --------OLD FASHIONED SUBROUTINES
hurtj:
HolderJ = j
GoTo doggy
hurtK:
HolderJ = k
doggy:
Sprite(HolderJ).flickerTime = clock + Sprite(HolderJ).invTime: PlaySound Sprite(HolderJ).soundFile

Return

// Thomas hits a goomba-
// ----------------------------------------------------------------------------
//      THOMAS HITS CRAP
// ----------------------------------------------------------------------------
If Sprite(j).name = "Thomas" Then   // ----------------------DAVID IF CHUNK
If Sprite(k).name = "goomba" Then
If hitdetection(j, k) = 1 And Sprite(j).z > Sprite(k).length And Sprite(j).lastJump > Sprite(j).z And Sprite(k).flickerTime < clock Then
Sprite(k).hp = Sprite(k).hp - 1: Sprite(k).flickerTime = clock + 2
If Sprite(k).hp < 1 Then

Sprite(k).name = "Dying Explosion"
Sprite(k).seekx = Sprite(11).wide * 2
Sprite(k).mph = 2
PlaySound 1
End If

End If
End If

If Sprite(k).name = "Deadly Rat" And hitdetection(j, k) = 1 Then
If Sprite(j).flickerTime < clock Then
Sprite(j).hp = Sprite(j).hp - 1: Sprite(j).flickerTime = clock + 2
If Sprite(j).hp < 1 Then Sprite(j).name = "Dying Explosion": Sprite(j).mph = 2: Sprite(j).seekx = Sprite(j).wide * 5
End If

End If
End If
// ----------------------------------------------------------------------------
//         END OF THOMAS HITS CRAP
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         FIREBALL TIME!
// ----------------------------------------------------------------------------
If Sprite(j).name = "fireball" Then
If Sprite(k).name = "Deadly Rat" Then
If Sprite(k).name <> "Dying Explosion" And Sprite(k).flickerTime < clock Then
Sprite(k).hp = Sprite(10).hp - 1
Sprite(k).flickerTime = clock + 2
If Sprite(k).hp < 1 Then
Sprite(k).name = "Dying Explosion"
Sprite(k).seekx = Sprite(k).wide * 2
Sprite(k).mph = 2
End If
Sprite(j).visible = False
End If
End If

End If
// ----------------------------------------------------------------------------
//         END OF FIRE BALL TIME!
// ----------------------------------------------------------------------------



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

Public Sub loadLevel(file As String)
Dim j As Integer
Dim texFile(9) As String
Dim texwide(9) As Integer
Dim texhigh(9) As Integer


file = levelPath + file
Open file For Input As #1
For j = 0 To 9
If j > 3 Or j < 1 Then
Input #1, texFile(j), texwide(j), texhigh(j)
view.loadTexture(j, texFile(j), texwide(j), texhigh(j));
End If
Next j
For j = 40 To 100
With Sprite(j)
Input #1, .name, .x, .y, .z, .srcx, .srcy, .srcx2, .srcy2, .wide, .high, .length, .texture, .visible, .kind, .zOrder
End With
Next j
Close #1
Call findOrder
End Sub

Sub debugLevel()
Call NowLoading
StopSound 0
destroyEverything

Dim j As Integer

CameraX = 0: CameraY = 0
view.loadTexture(-1, "lv1bg.bmp", 10, 10);
view.loadTexture(0, "smile.bmp", 255, 255);
view.loadTexture(1, "flip1.bmp", 254, 254);
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
Sub initPlayers()
// 1 Only player 1
// 2 Player 1 and 2
// 3 All three Players
// 4 Just player 2
// 5 Just player 3
// 6 Players 1 and 3
// 7 Players 2 and 3
Dim j As Integer
Sprite(0).name = "dead"
Sprite(10).name = "dead"
Sprite(20).name = "dead"

If numberPlayers = 1 Then gotFocus = 0
If numberPlayers = 2 Then gotFocus = -2
If numberPlayers = 3 Then gotFocus = -3
If numberPlayers = 4 Then gotFocus = 10
If numberPlayers = 5 Then gotFocus = 5

// Dim j As Integer
Dim k As Integer
For k = 0 To 2
If playerName(k) = "" Then playerName(k) = "redead"
Next k
// For j = 0 To (numberPlayers - 1)'
//
// 'Sprite(j * 10).x = 50
// Sprite(j * 10).y = 220
// Sprite(j * 10).name = playerName(j)
// For k = j * 10 + 1 To j * 10 + 9
// Sprite(k).x = 60
// Sprite(k).y = 70
// Sprite(k).name = "fireball"
// Next k
// Next j

// Rem****************88
// After killing the players above by setting the sprites (bodies) to "dead",
// it reincarnates them here by calling createPlayer
// Rem*************************************8

view.loadTexture(0, "System.bmp", 336, 397);
For k = 0 To 20 Step 10
// For penguin = 0 To 2
Call createPlayer(k, playerName(k / 10))
Next k


// Rem******************************************************8
//           THIS PART LOADS UP WEAPONS
// Rem******************************************************8


If numberPlayers = 1 Or numberPlayers = 2 Or numberPlayers = 3 Or numberPlayers = 6 Then
Sprite(0).name = playerName(0)
For k = 0 + 1 To 9
Sprite(k).x = 60
Sprite(k).y = 70
If playerName(0) = "Thomas" Or playerName(0) = "Nick" Then Sprite(k).name = "fireball"
If playerName(0) = "Nicky" Then Sprite(k).name = "bomb"
Next k
End If

If numberPlayers = 2 Or numberPlayers = 3 Or numberPlayers = 4 Or numberPlayers = 7 Then
Sprite(10).name = playerName(1)
For k = 11 + 1 To 19
Sprite(k).x = 60
Sprite(k).y = 70
If playerName(1) = "Thomas" Or playerName(1) = "Nick" Then Sprite(k).name = "fireball"
If playerName(1) = "Nicky" Then Sprite(k).name = "bomb"


Next k
End If

If numberPlayers = 3 Or numberPlayers = 5 Or numberPlayers = 6 Or numberPlayers = 7 Then
Sprite(20).name = playerName(2)
For k = 21 + 1 To 29
Sprite(k).x = 60
Sprite(k).y = 70
Sprite(k).name = "fireball"
Next k
End If


// Rem- THIS PART MAKES SPRITES DIFFERENT COLORS
If Sprite(0).name = Sprite(10).name Then Sprite(10).color = QBColor(10)


// Rem- This loads sounds into holders 0 to 14, meaning each player gets 5 sounds
// Rem- Spring.wav is then put into slot 15
For j = 0 To 2

If playerName(j) = "Thomas" Then
LoadSound (j * 5), "fireball.wav", "fireball"
LoadSound (j * 5) + 1, "death.wav", "DavidDeath"
LoadSound (j * 5) + 2, "hurt.wav", "DavidHurt"
End If

If playerName(j) = "Nicky" Then
LoadSound (j * 5), "NickyDeath.wav", "NickyDeath"
LoadSound (j * 5) + 1, "NickyHurt.wav", "NickyHurt"
LoadSound (j * 5) + 2, "setbomb.wav ", "set bomb"
LoadSound (j * 5) + 3, "Bomb Explode.wav", "bomb explode"
End If

If playerName(j) = "Nick" Then
LoadSound (j * 5), "nickdeath.wav", "nickdeath"
LoadSound (j * 5) + 1, "nickhurt.wav", "nickhurt"
LoadSound (j * 5) + 2, "iceshot.wav", "iceshot"
End If


Next j

LoadSound 15, "spring.wav", "spring"


End Sub
Sub selectPlayer()
//  the select player screen

destroyEverything
Call NowLoading
Call updatesprites
view.loadTexture(0, "players2.bmp", 320, 400);
view.loadTexture(-1, "PlayerS.bmp", 320, 240);
CameraWidth = 320: CameraHeight = 240

With Sprite(0)
.x = 2 * 2: .y = 36 * 2
.wide = 105 * 2: .high = (217 - 36) * 2
If anyKey(0) = 1 Then .visible = True
.name = "Selecter"
.frame = 1
.miscTime = clock + 2

End With
With Sprite(1)
.x = 106 * 2: .y = 36 * 2
.wide = 105 * 2: .high = (217 - 36) * 2
If anyKey(1) = 1 Then .visible = True
.name = "Selecter"
.frame = 2: .miscTime = clock + 2
End With
With Sprite(2)
.x = 212 * 2: .y = 36 * 2
.wide = 105 * 2: .high = (217 - 36) * 2
If anyKey(2) = 1 Then .visible = True
.name = "Selecter": .miscTime = clock + 2
.frame = 3
End With
Call loadAnimation(0, "selector.ani")
Call loadAnimation(1, "selector.ani")
Call loadAnimation(2, "selector.ani")

playBGM "Player Select.wav"
playWave "Select your characters of justice.wav"

End Sub

Function findQ(who As String) As Integer
Dim opera As Integer
Dim goatX As Integer


goatX = 0
For opera = 0 To spritesInUse
If Sprite(opera).name = who Then Let goatX = goatX + 1
Next opera

findQ = goatX
End Function

Function killS(goatX As Integer)
With Sprite(goatX)
.visible = False
.kind = 0
.name = ""
.trueVisible = 2
.flickerTime = 0
.target = -1

    With .SpriteVerts(0)
      .x = Sprite(goatX).x
      .y = Sprite(goatX).y + Sprite(goatX).high
      .tu = 0: .tv = 0.5
      .rhw = 1
      .color = normColor
    End With
    With .SpriteVerts(1)
      .x = Sprite(goatX).x
      .y = Sprite(goatX).y
      .tu = 0: .tv = 0
      .rhw = 1
      .color = normColor
    End With
    With .SpriteVerts(2)
      .x = Sprite(goatX).x + Sprite(goatX).wide
      .y = Sprite(goatX).y + Sprite(goatX).high
      .tu = 0.5: .tv = 0.5
      .rhw = 1
      .color = normColor
    End With
    With .SpriteVerts(3)
      .x = Sprite(goatX).x + Sprite(goatX).wide
      .y = Sprite(goatX).y
      .tu = 0.5: .tv = 0
      .rhw = 1
     .color = normColor
    End With
End With

End Function
Sub loadAnimation(who As Integer, file As String)
Dim j
Open FilePath + file For Input As #1
For j = 1 To 20
With Sprite(who)
Input #1, .Aframe(j).x
If .Aframe(j).x = -1 Then Exit For
Input #1, .Aframe(j).y, .Aframe(j).x2, .Aframe(j).y2
End With
Next j
Close #1

End Sub

Sub selectPlayerS()
//  after you all select players, it gets up the results

Dim penguin As Integer
If Sprite(0).mode = "done" Or Sprite(0).visible = False Then
If Sprite(1).mode = "done" Or Sprite(1).visible = False Then
If Sprite(2).mode = "done" Or Sprite(2).visible = False Then
For penguin = 0 To 2: If Sprite(penguin).visible = False Then Sprite(penguin).mode = ""
Next penguin
If Sprite(0).mode = "done" Then numberPlayers = 1
If Sprite(1).mode = "done" Then numberPlayers = 4
If Sprite(2).mode = "done" Then numberPlayers = 5
If Sprite(0).mode = "done" And Sprite(1).mode = "done" Then numberPlayers = 2
If Sprite(0).mode = "done" And Sprite(2).mode = "done" Then numberPlayers = 6
If Sprite(1).mode = "done" And Sprite(2).mode = "done" Then numberPlayers = 7
If Sprite(0).mode = "done" And Sprite(1).mode = "done" And Sprite(2).mode = "done" Then numberPlayers = 3

For penguin = 0 To 2

With Sprite(penguin)


// Next penguin


If .frame = 1 Then playerName(penguin) = "Thomas"
If .frame = 2 Then playerName(penguin) = "Nick"
If .frame = 3 Then playerName(penguin) = "Andrew"
If .frame = 4 Then playerName(penguin) = "Phil"
If .frame = 5 Then playerName(penguin) = "Nicky"
End With
Next penguin
// 1 Only player 1
// 2 Player 1 and 2
// 3 All three Players
// 4 Just player 2
// 5 Just player 3
// 6 Players 1 and 3
// 7 Players 2 and 3

screen = "level1.1"
End If
End If
End If

End Sub
Sub script()
Dim penguin As Integer
If Sprite(30).mode <> "3" And Sprite(30).mode <> "2" Then
// move all the faces so that they line up
Sprite(31).x = CameraX //  - Sprite(31).seekx
Sprite(31).y = CameraY + 20
Sprite(32).x = CameraX + CameraWidth - 268 // + Sprite(32).seekx
Sprite(32).y = CameraY + 20
Sprite(33).x = CameraX //  - Sprite(33).seekx
Sprite(33).y = CameraY + CameraHeight - 180
Sprite(34).x = CameraX + CameraWidth - 268 //  + Sprite(33).seekx
Sprite(34).y = CameraY + CameraHeight - 180
End If


If Sprite(30).mode = "6" Then
GoSub level1c

End If

If Sprite(30).mode = "5" Then

GoSub level1b
Sprite(30).mode = "6"
End If
If Sprite(30).mode = "4" And Sprite(30).miscTime < clock Then
If cinemaCounter >= cinemaMax Then Sprite(30).mode = "5" Else GoSub level1a
// Sprite(30).mode = "5"
End If
If Sprite(30).mode = "2" Then
If cinemaCounter > cinemaMax Then Sprite(30).mode = "9": Sprite(31).visible = False: Exit Sub

GoSub setUpLevel1
Sprite(30).mode = "3"
End If
If Sprite(30).mode = "3" Then

// Sprite(31).x = CameraX + 300
// Sprite(31).y = CameraY + 200
// Sprite(31).frame = 10
// Sprite(31).wide = 275 - 188
// Sprite(31).high = 376 - 353



Sprite(31).x = CameraX - Sprite(31).seekx
Sprite(31).y = CameraY + 20
Sprite(32).x = CameraX + CameraWidth - 268 + Sprite(32).seekx
Sprite(32).y = CameraY + 20
Sprite(33).x = CameraX - Sprite(33).seekx
Sprite(33).y = CameraY + CameraHeight - 180
Sprite(34).x = CameraX + CameraWidth - 268 + Sprite(33).seekx
Sprite(34).y = CameraY + CameraHeight - 180

If Sprite(31).seekx < 0 Then
Sprite(30).mode = "4"
Sprite(31).seekx = 0
Sprite(32).seekx = 0
Sprite(33).seekx = 0
Sprite(34).seekx = 0
End If

For penguin = 31 To 34
If penguin = 96 Then GoTo kiddy
Sprite(penguin).seekx = Sprite(penguin).seekx - sFactor * 6
Sprite(penguin).frame = Sprite(penguin).frame + 1
If Sprite(penguin).frame > 2 Then Sprite(penguin).frame = 1
// Sprite(penguin).frame = Sprite(penguin).seeky
kiddy:
Next penguin

End If
Exit Sub

setUpLevel1:
For penguin = 31 To 34
If penguin = 30 Then GoTo doggy
Sprite(penguin).visible = True
Sprite(penguin).wide = (271 - 137) * 2
Sprite(penguin).high = 81 * 2
Sprite(penguin).seekx = 268
Sprite(penguin).seeky = 1
Sprite(penguin).texture = 9
doggy:
Next penguin
Return

level1a:

Sprite(31).frame = cinema(cinemaCounter).frame1
Sprite(32).frame = cinema(cinemaCounter).frame2
Sprite(33).frame = cinema(cinemaCounter).frame3
Sprite(34).frame = cinema(cinemaCounter).frame4
Sprite(31).color = QBColor(cinema(cinemaCounter).color1)
Sprite(32).color = QBColor(cinema(cinemaCounter).color2)
Sprite(33).color = QBColor(cinema(cinemaCounter).color3)
Sprite(34).color = QBColor(cinema(cinemaCounter).color4)

playIsoWave (cinema(cinemaCounter).wavefile)
Sprite(30).miscTime = clock + cinema(cinemaCounter).miscTime
cinemaCounter = cinemaCounter + 1
Return

level1b:
For penguin = 31 To 34
If penguin = 30 Then GoTo doggy2
Sprite(penguin).visible = True
Sprite(penguin).wide = (271 - 137) * 2
Sprite(penguin).high = 81 * 2
Sprite(penguin).seekx = 0
Sprite(penguin).seeky = 1
Sprite(penguin).texture = 9
doggy2:
Next penguin
Return

level1c:
Sprite(31).x = CameraX - Sprite(31).seekx
Sprite(31).y = CameraY + 20
Sprite(32).x = CameraX + CameraWidth - 268 + Sprite(32).seekx
Sprite(32).y = CameraY + 20
Sprite(33).x = CameraX - Sprite(33).seekx
Sprite(33).y = CameraY + CameraHeight - 180
Sprite(34).x = CameraX + CameraWidth - 268 + Sprite(33).seekx
Sprite(34).y = CameraY + CameraHeight - 180

For penguin = 31 To 34
// If penguin = 34 Then GoTo kiddy2
Sprite(penguin).seekx = Sprite(penguin).seekx + sFactor * 6
Sprite(penguin).seeky = Sprite(penguin).seeky + 1: If Sprite(penguin).seeky = 3 Then Sprite(penguin).seeky = 1
Sprite(penguin).frame = Sprite(penguin).seeky
kiddy2:
Next penguin

If Sprite(32).seekx > 268 Then
For penguin = 31 To 34
Sprite(penguin).visible = False
Next penguin

Sprite(30).mode = "9"

End If

Return

End Sub

Function setCinema(who As Integer, frame1 As Integer, frame2 As Integer, frame3 As Integer, frame4 As Integer, color1 As Integer, color2 As Integer, color3 As Integer, color4 As Integer, wavefile As String, miscTime As Integer)
With cinema(who)
.frame1 = frame1
.frame2 = frame2
.frame3 = frame3
.frame4 = frame4
.color1 = color1
.color2 = color2
.color3 = color3
.color4 = color4
.wavefile = wavefile
.miscTime = miscTime
End With
End Function


Sub cinemaM(what As Integer)
// Everyone raise your hand who remembers cinemaM?
// Yes this classic far too long subroutine has made a triumphant return from Lady Pousha Quest.
If what = 2 Then
Sprite(30).name = "script": Sprite(30).mode = "2"
cinemaMax = 4
cinemaCounter = 0
setCinema 0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1b1.wav", 2.24
setCinema 1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1b2.wav", 2.74
setCinema 2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1b3.wav", 4.24
setCinema 3, 5, 3, 7, 16, 15, 15, 15, 8, "TalkLv1b4.wav", 0.95

End If


End Sub
Sub MakeLevel(lvlBgMusic As String, levelFile As String, levelBgFile As String, lvlBgWidth As Integer, lvlBgHeight As Integer, CinemaBitMapFile As String, CinemaAnimationFile As String, stopMusic As Boolean, loadScreen As Boolean)

// destroyEverything

destroyEverything 2


If loadScreen = True Then Call NowLoading

CameraWidth = 640: CameraHeight = 480

If stopMusic = True Then playBGM ""


// playBGM ""

// destroyEverything 2
loadLevel levelFile // "level1b.cap"

Gravity = 20

Dim j As Integer

CameraX = 0: CameraY = 0
view.loadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight); // "lv1bg2.bmp", 10, 10)
// Call loadTexture(0, "smile.bmp", 255, 255)

// Call loadTexture(4, "level1a.bmp", 490, 209)
// Call loadTexture(5, "lv1bgtw.bmp", 308, 341)
// Call loadTexture(6, "goomba.bmp", 240, 240)
view.loadTexture(9, CinemaBitMapFile, 400, 400); // "lvl1bg.bmp",400,400


Call initPlayers

// Call initPlayers
// For j = 0 To 2
// lives(j) = 3
// continues(j) = 3
// Next j


For j = 0 To spritesInUse
Call initSprites(j)
Next j

Call findOrder
Sprite(33).name = "cinema": Sprite(33).zOrder = -149
Sprite(32).name = "cinema": Sprite(32).zOrder = -149
Sprite(31).name = "cinema": Sprite(31).zOrder = -149
Sprite(34).name = "cinema": Sprite(34).zOrder = -149
Sprite(30).zOrder = -149
Call loadAnimation(33, CinemaAnimationFile)
Call loadAnimation(32, CinemaAnimationFile)
Call loadAnimation(31, CinemaAnimationFile)
Call loadAnimation(34, CinemaAnimationFile)
Sprite(30).name = "StageWhat": Sprite(30).mode = ""












If stopMusic = True Then playBGM lvlBgMusic


exitS = ""
Exit Sub

End Sub

Sub createPlayer(who As Integer, what As String)
Dim goatorg As Integer
With Sprite(who)

If playerName((who / 10)) = "Thomas" Then
weapon(who / 10) = "fireball"
Call loadAnimation(who, "thomas.ani")
view.loadTexture((who / 10) + 1, "flip1.bmp", 254, 254);
Sprite(who).texture = (who / 10) + 1
For goatorg = (who + 1) To (who + 9): Call loadAnimation(goatorg, "fireball.ani"): Next goatorg
End If

If playerName((who / 10)) = "Nick" Then
weapon(who / 10) = "fireball"
Call loadAnimation(who, "nick.ani")
view.loadTexture((who / 10) + 1, "joel.bmp", 254, 258);
Sprite(who).texture = (who / 10) + 1
For goatorg = (who + 1) To (who + 9): Call loadAnimation(goatorg, "icespike.ani"): Next goatorg
End If


If playerName((who / 10)) = "Nicky" Then
weapon(who / 10) = "bomb"
loadAnimation who, "nicky.ani"
view.loadTexture((who / 10) + 1, "LilNicky.bmp", 84, 148);
Sprite(who).texture = (who / 10) + 1
For goatorg = (who + 1) To (who + 9): Call loadAnimation(goatorg, "bomb.ani"): Next goatorg
End If
End With

For goatorg = (who + 1) To (who + 9): Sprite(goatorg).name = "": Sprite(goatorg).zOrder = -90: Next goatorg
// Sprite(who).name = what'playerName(who / 10)
// Sprite(who).frame = 1
// Call initSprites(who)

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
Sprite(which).jumpTime = clock
End Function

Function pickTarget(who As Integer, koo As Integer) As Integer
// Finds who is closet
// pickTarget = 8999
// 'cr As Integer
// crow As Integer
// For cr = 0 To NUMSPRITES
// If Sprite(cr).kind = koo Then
// crow = Abs((Sprite(cr).x + (Sprite(cr).wide / 2)) - Sprite(who).x) + Abs((Sprite(cr).y + Sprite(cr).high / 2) - Sprite(who).y)
// amount of pixels they are close
// If crow < pickTarget Then pickTarget = crow
// End If
// Next cr
// If pickTarget = 8999 Then pickTarget = -1
End Function

Function getMiddleX(who As Integer) As Double
getMiddleX = Sprite(who).x + (Sprite(who).wide / 2)
End Function
Function getMiddleY(who As Integer) As Double
getMiddleY = Sprite(who).y + (Sprite(who).high / 2)
End Function
Function getProx(who As Integer, who2 As Integer) As Double
// Finds who is closet

getProx = Abs(getMiddleX(who) - getMiddleX(who2)) + Abs(getMiddleY(who) - getMiddleY(who))
// amount of pixels they are close
End Function
Function seeker(who As Integer)
With Sprite(who)
If .x < .seekx Then .x = .x + (.mph * sFactor)
If .x > .seekx Then .x = .x - (.mph * sFactor)
If .y < .seeky Then .y = .y + (.mph * sFactor)
If .y > .seeky Then .y = .y - (.mph * sFactor)
End With
End Function

Sub levelR(which As Integer, who As Integer)
Dim k As Integer


With Sprite(who)

// LEVEL 1****************************************************************
If which = 1 Then  // LEVEL 1 ****************************

// If .name = "bullet" Then

// End If
If .name = "greenspring" Then
If .mode = "bounce" Then
.frame = .frame + 1
If .frame > 5 Then .frame = 2
If .miscTime < clock Then .mode = "": .frame = 1
End If
End If


If .name = "clouds" Then
.srcx = .srcx + (sFactor * 0.5)
.srcx2 = .srcx2 + (sFactor * 0.5)
.Aframe(1).x = .Aframe(1).x + 1
.Aframe(1).x2 = .Aframe(1).x2 + 1
End If



If .name = "bullet" Then     // 'This is a strange type of bullet that in retrospect feels more like a bubble
// If .seekx <> -1 Then

killLimit who
offCameraKill who


// If .mode = "" Then


Do Until (.seekx > cameraStopX Or .seekx < 0) Or (.seeky > cameraStopY Or .seeky < 0)
If .seekx > .x Then
.seekx = .seekx + ((.seekx - .x))
End If
If .seekx < .x Then
.seekx = .seekx - ((.x - .seekx))
End If

If .seeky > .y Then
.seeky = .seeky + ((.seeky - .y))
End If
If .seeky < .y Then
.seeky = .seeky - ((.y - .seeky))
End If
Loop



seeker who
.frame = .frame + 1: If .frame > 1 Then .frame = 0
End If


If .name = "paulrun" Then
If .mode = "" Then .mode = "right"

If .mode = "right" Then
.reverse = False
.x = .x + sFactor
.seekx = .seekx + sFactor
If .seekx > 100 Then .mode = "left": .seekx = 0: .dir = ""
End If
If .mode = "left" Then
.reverse = True
.x = .x - sFactor
.seekx = .seekx + sFactor
If .seekx > 100 Then .mode = "right": .seekx = 0: .dir = ""
End If

If .seekx >= 50 And .dir <> "done" Then Call shoot(who, "paulbullet", Sprite(checkProx(who)).x, Sprite(checkProx(who)).y): .dir = "done"
End If

If .name = "bluestick" Then
k = Int(Rnd * 2) + 1
If k = 1 Then .x = .x + sFactor
If k = 2 Then .x = .x - sFactor
k = Int(Rnd * 2) + 1
If k = 1 Then .y = .y + sFactor
If k = 2 Then .y = .y - sFactor
k = Int(Rnd * 20) + 1
If k = 1 Then If .z = 0 Then .jumpStart = .z: .jumpTime = clock

End If


If .name = "pigeonbomber" Then
.z = .z + sFactor
// .frame = .frame + 1
// If .frame > 2 Then .frame = 1

seeker who
If .x < 1 Then .x = cameraStopX

If .miscTime < clock Then
shoot who, "bluestick", Sprite(checkProx(who)).x, Sprite(checkProx(who)).y
.miscTime = clock + 2
End If

End If

Exit Sub
End If

End With
End Sub
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

Function offCameraKill(jex As Integer)
With Sprite(jex)
If .x > CameraX + 640 Or (.x + .wide) < CameraX Then killS jex

If .y > CameraY + 480 Or (.y + .high) < CameraY Then killS jex
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

Function printframes(who As Integer)
With Sprite(who)
Dim pigeon As Integer

For pigeon = 0 To 16
Debug.Print Str$(pigeon) + ": " + Str$(.Aframe(pigeon).x) + "," + Str$(.Aframe(pigeon).y) + "," + Str$(.Aframe(pigeon).x2) + "," + Str$(.Aframe(pigeon).y2)
Next pigeon
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
