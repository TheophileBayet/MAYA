import maya.cmds as cm

def off_translation(filename):
	f = open('filename', 'r')
	line = f.readline()
	line = f.readline()
	if line[0] = '#':
		line = f.readline()
		if line[0] = '#':
			line = f.readline()
	nb_vert = line[0]
	nb_faces = line[1]
	list_vert = []
	list_faces = []
	for i in range(0,nb_vert):
		line = f.readline()
		coord = [line[0],line[1],line[2]]
		list_vert.append(coord)
	line = readline()
	nb_vert_face = line[0]
	for i in range(0,nb_faces):
		points = [list_vert[line[1]],list_vert[line[2]],list_vert[line[3]]]
		list_faces.append(points)
		line = f.readline()
	
	f.close()
	print(list_vert)
