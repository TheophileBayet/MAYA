#import maya.cmds as cm

#def off_translation(filename):
f = open('../tux_lowres.off', 'r')
line = f.readline()
line = f.readline()
if line[0] == '#':
	line = f.readline()
	if line[0] == '#':
		line = f.readline()
line = line.split(' ')
nb_vert = int(line[0])
nb_faces = int(line[1])
list_vert = []
list_faces = []
for i in range(0,nb_vert):
	line = f.readline()
	line = line.split(' ')
	coord = [line[0],line[1],line[2]]
	list_vert.append(coord)
line = f.readline()
line = line.split(' ')
nb_vert_face = line[0]
for i in range(0,nb_faces):
	index1, index2, index3 = int(line[1]), int(line[2]), int(line[3])
	points = [list_vert[index1],list_vert[index2],list_vert[index3]]
	list_faces.append(points)
	line = f.readline()
	line = line.split(' ')
f.close()
for i in range (10):
	print(list_faces[i])
#print(list_vert)
