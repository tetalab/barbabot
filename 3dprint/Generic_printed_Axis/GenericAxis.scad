/*
Generic X /Y /Z axis
I suppose this could be use in any linear motion control application.
I am designing this specifically to be the Z-Axis for my 3D UV printer

This design uses 3/8"  (9.525) rod, because that's what I have laying around.
I also have some generic ~1/2 OD bushings I'm going to use on the shuttle

I'm also utilizing some 1/4 (8mm) 20tpi threaded rod 
Overall length is determined by rod length


*/

// The following variables can be changed to any size
rodsize = 9.525; // 3/8 rod
bushingOD = 12.7; //the 1/2 OD of the bushings I have
ThreadedRodOD = 8;// the 1/4 20tpi threaded rod
boltsize = 6; // assume 6 mm bolts to hold this to the base


module smallnut()
{
	cylinder(h=3,r=8/2,$fn=6);
}

// a common base that both the left and right half are derived from
module BaseCommon()
{
	difference()
	{
		union()
		{
			cube(size=[70,10,50],center = true);

			translate(v=[0,25,-20])			
				cube(size=[70,50,10],center = true);

			translate(v=[-27,-5,16])		
				rotate(a=[-90,0,0]) // left rod
					cylinder(h=20,r=(rodsize*1.4)/2);

			translate(v=[27,-5,16])		
				rotate(a=[-90,0,0]) // left rod
					cylinder(h=20,r=(rodsize*1.4)/2);


			translate(v=[70/2-4,20,0])		
				rotate(a=[0,-90,0]) // left rod
					RightTriangle(30,30,8);

			translate(v=[-70/2+4,20,0])		
				rotate(a=[0,-90,0]) // left rod
					RightTriangle(30,30,8);

		}
		translate(v=[27,0,20])		
				smallnut();
		translate(v=[27,0,20])		
				cylinder(h=10,r=3/2,$fn=33);

		translate(v=[-27,0,20])		
				smallnut();
		translate(v=[-27,0,20])		
				cylinder(h=10,r=3/2,$fn=33);

		//take out the place for the holes
		HolePattern();

		translate(v=[-27,-15,16]) // hollow out the rod holders		
			rotate(a=[-90,0,0]) // left rod
				cylinder(h=30,r=(rodsize)/2);

		translate(v=[27,-15,16]) // hollow out the rod holders		
			rotate(a=[-90,0,0]) // left rod
				cylinder(h=30,r=(rodsize)/2);

	}

}
//RightTriangle(30,30,10);
module RightTriangle(width,height,thick)
{
	difference()
	{
	
		cube(size=[width,height,thick],center = true);
		translate(v=[(width/2) - ( 0.3333 * width),height/2,0])
			rotate(a=[0,0,45])
				cube(size=[width,height*2,thick*2],center = true);
	}
}

// The section with the stepper motor
// this is going to be somewhat low-profile and kind of hefty
module LeftHalf()
{
	difference()
	{
		BaseCommon();
		//take out a place for the stepper mount
		translate(v=[0,7,5])
			rotate(a=[90,0,0]) 
				steppermount();
	}
}

// The section with the idler bearing

module RightHalf()
{
	difference()
	{
		BaseCommon();

		//space for a 608 bearing
		translate(v=[0,9,5])
			rotate(a=[90,0,0]) 
				cylinder(h=12,r=22/2);

		//and a hole for the center threaded rod
		translate(v=[0,12,5])
			rotate(a=[90,0,0]) 
				cylinder(h=30,r=(ThreadedRodOD+3)/2);

	}
}
module HolePattern()
{
	translate(v=[20,40,-30])			
			cylinder(h=20,r=boltsize/2,$fn=33);
	translate(v=[-20,40,-30])			
			cylinder(h=20,r=boltsize/2,$fn=33);
	translate(v=[20,20,-30])			
			cylinder(h=20,r=boltsize/2,$fn=33);
	translate(v=[-20,20,-30])			
			cylinder(h=20,r=boltsize/2,$fn=33);
}

module Rails()
{	
	translate(v=[-27,100,0])
		rotate(a=[90,0,0])
			cylinder(h=200,r=rodsize/2,$fn=33);
	translate(v=[27,100,0])
		rotate(a=[90,0,0])
			cylinder(h=200,r=rodsize/2,$fn=33);


	translate(v=[0,100,-11])
		rotate(a=[90,0,0])
			cylinder(h=200,r=ThreadedRodOD/2,$fn=33);
		
}

module nut8mm() // for the threaded rod
{
	cylinder(h=7,r=13/2,$fn=6);
}
module Axis()
{
	translate(v=[0,-100,0])
		LeftHalf();
	translate(v=[0,100,0])
		rotate(a=[0,0,180])
			RightHalf();
}
// this module makes the mounting hole for the Nema 1.7 motor
bspace = 31/2; // bolt spacing
div = 33; // divisions in a circle
module steppermount(cx=0,cy=0,cz=0,rad=3.2)
{
		//make a hole for the stepper
         		 translate(v=[cx,cy,cz])
          	{
	       		 cylinder(h = 30, r=14,$fn =div); // make it coupler - friendly
		}
		//bolts are 31 mm spaced from each other

         		 translate(v=[cx+bspace,cy+bspace,cz])
          	{
	       		 cylinder(h = 30, r=rad/2,$fn =div);
	       		 cylinder(h = 3, r=3,$fn =div);
		}	
         		 translate(v=[cx+bspace,cy-bspace,cz])
          	{
	       		 cylinder(h = 30, r=rad/2,$fn =div);
	       		 cylinder(h = 3, r=3,$fn =div);
		}	
         		 translate(v=[cx-bspace,cy+bspace,cz])
          	{
	       		 cylinder(h = 30, r=rad/2,$fn =div);
	       		 cylinder(h = 3, r=3,$fn =div);
		}	
         		 translate(v=[cx-bspace,cy-bspace,cz])
          	{
	       		 cylinder(h = 30, r=rad/2,$fn =div);
	       		 cylinder(h = 3, r=3,$fn =div);
		}	
}

// a rough approximation of a NEMA17 motor used for sizing purposes()
module NEMA17()
{
	union()
	{
		cube(size=[42,42,38],center = true);
		translate(v=[0,0,38/2])
			cylinder(h=2.5,r=11);
		translate(v=[0,0,38/2])
			cylinder(h=2.5,r=11);
		translate(v=[0,0,38/2])
			cylinder(h=13,r=2.5);
	}
}




module nutholder()
{
	difference()
	{
		union()
		{
			cube(size=[30,10,10],center = true);
		}
	}
}

module fakebolt()
{
	union()
	{
		cylinder(h=40,r= 5/2);
		cylinder(h=5,r= 8/2);
	}
}


module Slider()
{
	difference()
	{
		union()
		{
/*
			translate(v=[0,0,0])
				cube(size=[60,55,19],center = true);

			// the round holder parts for the bushings
			translate(v=[-27,55/2,0])
				rotate(a=[90,0,0])
					cylinder(h=55,r=(bushingOD*1.5)/2,$fn=33);
			translate(v=[27,55/2,0])
				rotate(a=[90,0,0])
					cylinder(h=55,r=(bushingOD*1.5)/2,$fn=33);

		*/
		translate(v=[0,0,-15])
			nutholder();
		

		}


		translate(v=[10,0,10])
			rotate(a=[180,0,0])
				fakebolt();

		translate(v=[-10,0,10])
			rotate(a=[180,0,0])
				fakebolt();


		translate(v=[0,7/2,-11])
			rotate(a=[90,0,0])
				nut8mm();

		// the OD cutouts of the bushings inside the bushing holders
		translate(v=[-27,51/2,0])
			rotate(a=[90,0,0])
				cylinder(h=51,r=(bushingOD)/2,$fn=33);
		translate(v=[27,51/2,0])
			rotate(a=[90,0,0])
				cylinder(h=51,r=(bushingOD)/2,$fn=33);
			// the cutouts for the rails
		translate(v=[-27,55/2,0])
			rotate(a=[90,0,0])
				cylinder(h=55,r=(rodsize+1.5)/2,$fn=33);
		translate(v=[27,55/2,0])
			rotate(a=[90,0,0])
				cylinder(h=55,r=(rodsize+1.5)/2,$fn=33);

		translate(v=[0,0,15])
			SliderHolePattern();
		// the threaded rod
		translate(v=[0,100,-11])
			rotate(a=[90,0,0])
				cylinder(h=200,r=(ThreadedRodOD/2)+1,$fn=33);

	}
}



module SliderHolePattern()
{
	holeXOffset =15;
	holeYOffset =15;
	translate(v=[holeXOffset,holeYOffset,-30])			
			cylinder(h=30,r=boltsize/2,$fn=33);
	translate(v=[-holeXOffset,holeYOffset,-30])			
			cylinder(h=30,r=boltsize/2,$fn=33);
	translate(v=[holeXOffset,-holeYOffset,-30])			
			cylinder(h=30,r=boltsize/2,$fn=33);
	translate(v=[-holeXOffset,-holeYOffset,-30])			
			cylinder(h=30,r=boltsize/2,$fn=33);
}
module main()
{
//	Axis();
	translate(v=[0,0,16])
	{
		Rails();
		Slider();
	}
}

//main();

module SliderBottom()
{
	difference(){
	Slider();
	translate(v=[0,0,5])
		cube(size=[80,80,10],center= true);
	}
}

LeftHalf();

