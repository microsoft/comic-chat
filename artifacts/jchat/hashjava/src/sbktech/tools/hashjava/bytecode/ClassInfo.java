/*
 * Copyright (C) 1996 KB Sriram
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
 * MA 02139, USA.
 */

package sbktech.tools.hashjava.bytecode;
import java.util.*;
import sbktech.tools.hashjava.util.QuickSort;
import java.io.*;

/**
 * The ClassInfo is the basic informational unit about a single class.
 * Once a class is read in, all information about it is stored,
 * manipulated and written out here.
 *
 * <p>ClassInfo is handed to the Obfuscator interface as a
 * way to obtain information about symbols defined in a Class file,
 * and also provide means to rename these symbols.
 *
 * @see Obfuscator#obfuscate
 * @author $Author: kbs $
 * @version $Revision: 1.11 $
 */

public class ClassInfo implements VMConstants, Modifiable
{
//////////////////////////////Public access/////////////////////////////

  /**
   * This returns the set of fields defined in the class,
   * and this is the only set that should be altered.
   * @return an array of Modifiables that represent the
   *         fields defined in this class
   */
   public Modifiable[] definedFields()
   {
     return vars;
   }

  /**
   * This returns the set of methods defined in the class,
   * and this is the only set that should be altered.
   * Overridden methods, or methods implemented from interfaces
   * are not considered to be defined in the class.
   * @return an array of Methods which represent the methods
   *         defined in this class
   */
   public Method[] definedMethods()
   {
     return defMethods;
   }

  /**
   * @param m is the method for which a dependency list is desired
   * @return a Hashtable of ClassInfo entries. The Hashtable is just
   * a set of all classes which will have a method renamed if
   * method m in this class is renamed. It will always return null if
   * this class is not an interface.
   */
  public Hashtable affects(Method m)
  {
    if (affectsTable == null) return null;

    return ((Hashtable)(affectsTable.get(m.getNtcp())));
  }

   /**
    * This adds a named attribute into a Class file.
    * It is just a way to embed arbitrary data into a class file.
    * You can use this for instance, to store copyright strings
    * or versioning information directly in the bytecode.
    *
    * @param name Name for the attribute
    * @param data bytes containing the data to be associated with
    *             the name
    */
  public void addAttribute(String name, byte[] data)
  {
    extraAttrs.addElement(new BasicAttr(name, data));
  }

  /**
   * Access permissions for this class
   * @return an integer (see values in VMConstants) describing
   *         the access permissions for this class
   * @see VMConstants
   */
  public int permissions()
  { return class_access; }

  /**
   * @return the name for the class
   */
  public String originalName()
  { return Sthis_class; }

  /**
   * @return the new (possibly renamed) name for this class
   */
  public String newName()
  {
    if (!isInner)
      {
        if (Snew_this_class != null)
          { return Snew_this_class; }
        return Sthis_class;
      }
                                // Inner class, so mangle name
                                // using innerclass and class names
    String pname = env.newClassName(parentScopeName);
    if (newInnerName != null)
      { return (pname + innerSeparator + newInnerName); }
    else
      { return (pname + innerSeparator + oldInnerName); }
  }

  /**
   * @return true if this class is an inner class
   */
  public boolean isInnerClass()
  { return isInner; }

  private boolean isInner = false;
  private String parentScopeName = null;
  private String  oldInnerName = null;
  private String newInnerName = null;
  private String innerSeparator = null;

  /**
   * @return the name of the containing class scope if this is
   * an inner class
   */
  public String parentScope()
  { return parentScopeName; }

  /**
   * @return the name of this inner class. This can
   * be null or 0 length if the class is not an inner class, or if the
   * class is anonymous.
   */
  public String newInnerName()
  { return ((newInnerName!=null)?newInnerName:oldInnerName); }

  /**
   * Set a new name for this class. If this is an inner class, this
   * only renames the "inner" name.
   */
  public void rename(String n)
  {
    if (!isInner)
      { Snew_this_class = n; }
    else
      { newInnerName = n; }
  }

  public String toString()
  { return originalName(); }

//////////////////////////////Package access////////////////////////////

  // Starting from a DataInput definition, extract all the information
  // relevent to this class. The environment is used to add and obtain
  // unique global identifiers for symbols defined in this class.

  ClassInfo (DataInput in, Environment env)
       throws IOException, BytecodeFormatException
  {
    this.env = env;

    magic = in.readInt();
    if (magic != JAVA_MAGIC)
      { throw new BytecodeFormatException("This isn't a class file"); }

    version_lo = in.readUnsignedShort();
    version_hi = in.readUnsignedShort();

    if (version_hi > JAVA_VERSION)
      {
        throw
          new BytecodeFormatException
          ("Cannot handle version > " + JAVA_VERSION
           + " (this version is " +  version_hi + ")");
      }

    int nCp = in.readUnsignedShort();
    //    System.out.println(nCp + " CP's");

    oldCps = new CP[nCp];     // overestimate, but easier
                                // to deal with setting indices

    for (int i=1; i<nCp; i++)
      {
        oldCps[i] = CP.readCP(in);
        if ((oldCps[i] instanceof LongCP) ||
            (oldCps[i] instanceof DoubleCP))
          { i++; }
      }

                                // Now slap in the real pointers
    for (int i=1; i<nCp; i++)
      {
        oldCps[i].setRefs(oldCps);
        //        System.out.println(i + ": " + oldCps[i]);
        if ((oldCps[i] instanceof LongCP) ||
            (oldCps[i] instanceof DoubleCP))
          { i++; }
      }

    class_access = in.readUnsignedShort();
    this_class = (ClassCP) oldCps[in.readUnsignedShort()];
    Sthis_class = this_class.Sname;
    super_class = (ClassCP) oldCps[in.readUnsignedShort()];


    interfaces = new ClassCP[in.readUnsignedShort()];
    for (int i=0; i<interfaces.length; i++)
      {
        interfaces[i] = (ClassCP)(oldCps[in.readUnsignedShort()]);
      }

    //    System.out.println(interfaces.length + " interfaces");

    vars = new Field[in.readUnsignedShort()];
    oldFields = new Hashtable(10);
    for (int i=0; i<vars.length; i++)
      {
        vars[i] = new Field(in, oldCps, env.stats);
                                // Create a dummy nametype entry
                                // to index this variable
        oldFields.put(new NameTypeCP(vars[i].originalName(),
                                     vars[i].getSig()),
                      vars[i]);
      }

    //    System.out.println(vars.length + " vars");

    methods = new Method[in.readUnsignedShort()];
    for (int i=0; i<methods.length; i++)
      { methods[i] = new Method(in, oldCps, env.stats); }
    //    System.out.println(methods.length + " methods");

    attrs = new Attr[in.readUnsignedShort()];
    //    System.out.println(tmpAttrs.length + " attrs");
    for (int i=0; i<attrs.length; i++)
      {
        attrs[i] = Attr.readAttr(in, oldCps, env.stats);
        if (attrs[i] instanceof InnerClassesAttr)
          { checkForInner((InnerClassesAttr) attrs[i]); }
        //        System.out.println(i + ": " + tmpAttrs[i]);
      }
  }


  // get the new name for a particular field (the field asked for
  // contains the original name). If the field is not declared in
  // this class, it returns a null.
  // NOTE: Should this signal an error instead?
  String newFieldName(NameTypeCP ntcp)
  {
    Field f = (Field) oldFields.get(ntcp);
    if (f == null)
      { return null; }
    return f.newName();
  }

  // set this if a method has already corrupted the local variable
  // table
  void setVarCorrupted(boolean state)
  { _varCorrupted = state; }
  // has this class already have a method with a corrupted local
  // variable?
  boolean varCorrupted()
  { return _varCorrupted; }


  // get the new name for a particular method. (the method asked for
  // contains the original name). If this class cannot modify the symbol,
  // the answer is asked from whichever superclass/interface actually
  // implements the method. If the method is not even declared in this
  // class, a null is returned.
  String newMethodName(NameTypeCP ntcp)
  {
    String ret = null;

    Method m = (Method) oldDefMethods.get(ntcp);
    if (m != null)
      {
        // This method has the right to alter the name
        return m.newName();
      }

    // If this class has been punted, get the answer from the punted
    // interface

    ClassInfo c = (ClassInfo)puntedMethods.get(ntcp);
    if (c != null)
      { return c.newMethodName(ntcp); }

    // Otherwise, search in superclass
    if (super_class != null)
      {
        if ((ret  = env.newMethodName
             (super_class.originalName(), ntcp))
            != null)
          return ret;
      }
    return null;
  }

  // once everything has been added to the Environment,
  // figure out what symbols are potentially renamable in this class.
  // This should take care of things like overridden methods.

  // A second pass is made later on to deal with interfaces. See
  // puntInterfaces() and mergeInterfaces()

  void resolveDefinitions()
  {
    oldDefMethods = new Hashtable(10);
    puntedMethods = new Hashtable(10);

                                // A non static method is considered to be
                                // overridden if it is defined in
                                // its superclass

  nextMethod:
    for (int i=0; i<methods.length; i++)
      {
        // Static methods are always particular to this class
        if ((methods[i].permissions() &
             ACC_STATIC) != 0)
          {
            oldDefMethods.put
              (new NameTypeCP(methods[i].originalName(),
                              methods[i].getSig()),
               methods[i]);
            continue nextMethod;
          }

        // An instance method. Don't add it if the superclass
        // defines it.
        if ((super_class != null) &&
            (env.forName(super_class.name.val).declaredMethodP(methods[i])))
          continue nextMethod;

        // Ok, add this one in. NB, it might still be declared
        // in an interface implemented by this class or a subclass, see
        // puntInterfaces() to see how this is handled.
        oldDefMethods.put
          (new NameTypeCP(methods[i].originalName(),
                          methods[i].getSig()),
           methods[i]);
      }
  }

  // This is applied to all classes once resolveDefinitions() is complete.
  // This phase is used to push methods to interfaces, if the class
  // declares one. As methods are pushed to interfaces, a link is
  // established between any different implemented interfaces with
  // a method in common.
  // A final pass (mergeInterfaces()) is then used to associate a
  // canonical interface with all the interfaces that have methods
  // in common.

  void puntInterfaces()
  {
    //    env.stats.verboseMessage("Punting interfaces from " + this);
    for (int i=0; i<interfaces.length; i++)
      {
        ClassCP curInterface = interfaces[i];
        checkInInterface(curInterface);
      }
  }


  // This is the last phase to obtain the defined methods for a class.
  // In this phase, any interfaces linked over a method are merged to
  // obtain a canonical interface for that method.
  void mergeInterfaces()
  {
    // If there are no equivalencies to any method, we are done
    if (equivalencies == null)
      return;

    for (Enumeration e = oldDefMethods.elements(); e.hasMoreElements();)
      {
        Method m = (Method)(e.nextElement());
        NameTypeCP ntcp = m.getNtcp();

        // If this method has already been punted, skip
        if (puntedMethods.get(ntcp) != null)
          continue;

        // Check if there are any other interfaces that
        // need to be made equivalent to this one for this
        // method
        Vector links = (Vector)equivalencies.get(m.getNtcp());
        if (links != null)
          {
            // Choose me to be the canonical representation for
            // this method, and fix up the targets accordingly
            for (Enumeration f=links.elements(); f.hasMoreElements();)
              {
                ClassInfo eqv = (ClassInfo)(f.nextElement());
                eqv.makeEquivalent(this, ntcp);
              }
          }
      }
  }

  // This phase is done after mergeInterfaces(). The purpose is
  // to create (for every interface) a list of all the classes
  // that are affected by a particular method.
  // For each method punted to an interface, the interface is
  // notified (via addAffected()) about this class.

  void notifyInterfaces()
  {
    for (Enumeration en=puntedMethods.keys(); en.hasMoreElements();)
      {
        NameTypeCP n = (NameTypeCP)(en.nextElement());
        ClassInfo c = (ClassInfo)(puntedMethods.get(n));
        if ((c.permissions() & ACC_INTERFACE) != 0)
          { c.addAffected(this, n); }
      }
  }

  // Create the defMethod arrays
  void makeArrays()
  {
    boolean isInterface;
    isInterface = (permissions() & ACC_INTERFACE) != 0;

    for (Enumeration e = puntedMethods.keys(); e.hasMoreElements();)
      {
        NameTypeCP n = (NameTypeCP)(e.nextElement());
        if (oldDefMethods.remove(n) != null)
          {
            if (!isInterface)
              throw new RuntimeException
                ("ASSERT FAILED: " + this + " removed method " +
                 n + " from a non interface");
          }
      }

    defMethods = new Method[oldDefMethods.size()];
    int i=0;
    for (Enumeration e = oldDefMethods.elements(); e.hasMoreElements();)
      {
        defMethods[i] = (Method) e.nextElement();
        i++;
      }
  }

  // Return any references from this class to any other class.
  // This includes the superclass, interfaces, and if resolveFully
  // is true, any references from within the class to other classes,
  // except for java.* packages.

  String[] getDependencies(boolean resolveFully)
  {
    if ((resolveFully) &&
        (!originalName().startsWith("java/")))
      { return getFullDependencies(); }
    else
      { return getMinimalDependencies(); }
  }

  // Write out the current definition of the class.
  // This assumes that the definitions have been resolved, and
  // that obfuscation has also completed.

  void write(DataOutput out)
       throws IOException
  {

                                // Generate all the symbols needed
                                // to create this class

    cpe = new Hashtable();

                                // First the local class references
    this_class.setNewName(env, this);
    this_class = (ClassCP) addCP(this_class);

    if (super_class != null)
      {
        super_class.setNewName(env, this);
        super_class = (ClassCP) addCP(super_class);
      }
    for (int i=0; i<interfaces.length; i++)
      {
        interfaces[i].setNewName(env, this);
        interfaces[i] = (ClassCP) addCP(interfaces[i]);
      }

                                // Add references from the newly named
                                // fields and methods.
    for (int i=0; i<vars.length; i++)
      { vars[i].makeCP(env, this); }
    for (int i=0; i<methods.length; i++)
      { methods[i].makeCP(env, this); }

                                // Finally, extra attributes to be added
    int nattrs = 0;
    for (int i=0; i<attrs.length; i++)
      {
        if (attrs[i] instanceof SourceFileAttr)
          {
            switch(env.sourceDebugging)
              {
              case env.DEBUG_REMOVE:
                attrs[i].include(false);
                break;
              case env.DEBUG_CORRUPT:
                attrs[i].corrupt();
                break;
              default:
                break;
              }
          }
        if (attrs[i].includeP())
          { attrs[i].makeCP(env, this); nattrs++; }
      }

    for (Enumeration e = extraAttrs.elements(); e.hasMoreElements();)
      {
        Attr at = (Attr)(e.nextElement());
        at.makeCP(env, this); nattrs++;
      }

                                // Now generate indices for the new CPs

                                // Add unicode strings first, and sort them
                                // so they compress better.
    Vector newCps = new Vector(cpe.size());

    int ucnt = 0;
    for (Enumeration e = cpe.elements(); e.hasMoreElements();)
      {
        CP curcp = (CP)(e.nextElement());
        if (curcp instanceof UnicodeCP)
          { newCps.addElement(curcp); ucnt++; }
      }
    QuickSort.sort(newCps, 0, ucnt-1);

                                // Push string constants to the head as a
                                // heuristic to reduce the size of
                                // opcodes (ldc_w -> ldc mostly)

    /*
    for (Enumeration e = cpe.elements(); e.hasMoreElements();)
      {
        CP curcp = (CP)(e.nextElement());
        if (curcp instanceof StringCP)
          newCps.addElement(curcp);
      }
      */
    for (Enumeration e = cpe.elements(); e.hasMoreElements();)
      {
        CP curcp = (CP)(e.nextElement());
        if (!(curcp instanceof UnicodeCP))
          newCps.addElement(curcp);
      }

    short curidx = 1;
    for (Enumeration e = newCps.elements(); e.hasMoreElements();)
      {
        CP curcp = (CP)(e.nextElement());
        curcp.idx = curidx;
        if ((curcp instanceof LongCP) ||
            (curcp instanceof DoubleCP))
          { curidx += 2; }
        else
          { curidx++; }
      }

                                // Now ready to write out data
    out.writeInt(magic);
    out.writeShort(version_lo);
    out.writeShort(version_hi);
    out.writeShort(curidx);
    for (Enumeration e = newCps.elements(); e.hasMoreElements();)
      {
        CP curcp = (CP)(e.nextElement());
        curcp.write(out);
      }
    out.writeShort(class_access);
    out.writeShort(this_class.idx);
    if (super_class != null)
      { out.writeShort(super_class.idx); }
    else
      { out.writeShort(0); }

    out.writeShort(interfaces.length);
    for (int i=0; i<interfaces.length; i++)
      { out.writeShort(interfaces[i].idx); }

    out.writeShort(vars.length);
    for (int i=0; i<vars.length; i++)
      { vars[i].write(out); }

    out.writeShort(methods.length);
    for (int i=0; i<methods.length; i++)
      { methods[i].write(out); }

    out.writeShort(nattrs);
    for (int i=0; i<attrs.length; i++)
      {
        if (attrs[i].includeP())
          { attrs[i].write(out); }
      }
    for (Enumeration e = extraAttrs.elements(); e.hasMoreElements();)
      {
        Attr at = (Attr)(e.nextElement());
        at.write(out);
      }
  }

  // add a CP to the new set of symbols in the Class.
  // This will also automatically add any other CP's this
  // CP needs through calling the resolve() interface in CP's
  // It returns a reference to the unique CP that is actually used
  // in the final symbol table
  CP addCP(CP cp)
  {
    CP ret;
    if ((ret = (CP) cpe.get(cp)) == null)
      {
                                // Add to existing pool of CP's
        cpe.put(cp, cp);
                                // Resolve it to load up dependent CP's
        cp.resolve(this);
        return cp;
      }
    return ret;
  }

//////////////////////////////Private access////////////////////////////

  private int magic;
  private int version_lo, version_hi;
  private int class_access;
  private ClassCP this_class, super_class;
  private String Sthis_class, Snew_this_class;
  private Hashtable cpe;
  private ClassCP interfaces[];
  private Field vars[];
  private Hashtable oldFields;  // Fields indexed by their name/types
  private Method methods[];

  private Method defMethods[];  // subset of methods which are
                                // actually defined in this class

                                // *Defined * methods indexed by
                                // their name/types
  private Hashtable oldDefMethods;
                                // *Defined* methods punted from this class
                                // into an interface
  private Hashtable puntedMethods;
                                // Vector list of equivalent interfaces indexed
                                // by name/type
  private Hashtable equivalencies = null;
                                // Hashtable of Hashtables, indexed by
                                // name/type.
  private Hashtable affectsTable = null;

  private Attr attrs[];
  private Vector extraAttrs = new Vector();
  private Vector generic;
  private Environment env;
  private CP oldCps[];          // Contains the original list of CP's
                                // in the class.
  private boolean _varCorrupted = false;

  private void checkInInterface(ClassCP cinterface)
  {
    ClassInfo cin = env.forName(cinterface.name.val);

    for (Enumeration e = cin.oldDefMethods.elements();
         e.hasMoreElements();)
      {
        Method m = (Method)(e.nextElement());
        // As it turns out, static methods *are* possible in
        // interfaces. (<clinit>)
        if ((m.permissions() & ACC_STATIC) == 0)
          {
            try { puntInterfaceMethod(cin, m); }
            catch(InterfaceMatchFailedException ex)
              {
                env.stats.info(originalName() + " does not appear to have any method " + m);
                env.stats.info("but it implements " + cin);
                throw (ex);
              }
          }
      }

    // propagate this also into the interfaces implemented by
    // this interface. We assume that there are no cycles.
    for (int i=0; i<cin.interfaces.length; i++)
      { checkInInterface(cin.interfaces[i]); }
  }



  // is a certain Method declared in this class?
  // This returns true if it is declared in this or a superclass

  private boolean declaredMethodP(Method m)
  {
                                // Silly linear search.
                                // NOTE: optimize
    for (int i=0; i<methods.length; i++)
      {
        if (m.equals(methods[i]))
          { return true; }
      }
                                // Check in superclass
    if ((super_class != null) &&
        (env.forName(super_class.name.val).declaredMethodP(m)))
      { return true; }

    return false;
  }

  // If this interface method is declared in this class, punt it to
  // the interface.

  private void puntInterfaceMethod(ClassInfo c, Method min)
  {
    if ((c.permissions() & ACC_INTERFACE) == 0)
      {
        throw new RuntimeException
          ("\n" + c + " is not an interface but\n" +
           this + " implements it one.\nThis may be because classes have not been completely recompiled.\n\nRecompile all your source code, and try obfuscating again.\nIf this problem still occurs, please file a bug report\nwith kbs@sbktech.org");
      }

    NameTypeCP ntcp = min.getNtcp();
    Method m;
    if ((m = (Method) oldDefMethods.get(ntcp)) != null)
      {
        oldDefMethods.remove(ntcp);
        //        env.stats.verboseMessage(this + " punting " + m + " to " + c);
        puntedMethods.put(ntcp, c);
        return;
      }

    // Check if it has already been punted
    ClassInfo target = null;
    if ((target = (ClassInfo) puntedMethods.get(ntcp)) != null)
      {
        // Check if the current target is the same as the already punted
        // method. If not, link up the two interfaces together for this
        // method.
        if (target != c)
          {
            c.linkTo(target, ntcp);
            target.linkTo(c, ntcp);
          }
        return;
      }

    // If the current class is an interface, its ok for it to
    // implement an interface method that isn't there
    if ((permissions() & ACC_INTERFACE) != 0)
      { return; }
    // Otherwise, pass the request to the superclass of the current class
    else if (super_class != null)
      { env.forName(super_class.name.val).puntInterfaceMethod(c, min); }
    else
      {
        throw new InterfaceMatchFailedException("Could not find " + min + " from interface " + c);
      }
  }

  // This is used in a DF traversal of the equivalency list
  // to associate a particular interface with a method
  private void makeEquivalent(ClassInfo c, NameTypeCP ntcp)
  {
    if ((permissions() & ACC_INTERFACE) == 0)
      {
        throw new RuntimeException("ASSERT failed: " + this + " is not an interface!");
      }

    // return on self references
    if (c == this) return;
    // Or if it has already been set (but verify this!)
    ClassInfo cme = (ClassInfo) puntedMethods.get(ntcp);
    if (cme != null)
      {
        if (cme != c)
          { throw new RuntimeException("ASSERT failed: " + c + "!=" +cme); }
        return;
      }

    // Put the reference and continue the setting on this interfaces
    // links on this method
    if (oldDefMethods.get(ntcp) != null)
      {
        puntedMethods.put(ntcp, c);
        Vector links = (Vector) equivalencies.get(ntcp);
        if (links != null)
          {
            for (Enumeration e = links.elements(); e.hasMoreElements();)
              {
                ClassInfo ci = (ClassInfo)(e.nextElement());
                ci.makeEquivalent(c, ntcp);
              }
          }
        return;
      }
    // Propagate the request to the superclass
    if (super_class != null)
      { env.forName(super_class.name.val).makeEquivalent(c, ntcp); }
    else
      { throw new RuntimeException("Interface does not define " + ntcp); }
  }

  // Link a particular method with another interface. This is used
  // to create a graph that links methods in interfaces that are
  // equivalent because a class implements both of them

  private void linkTo(ClassInfo target, NameTypeCP ntcp)
  {
    if ((permissions() & ACC_INTERFACE) == 0)
      {
        throw new
          RuntimeException("ASSERT failed: " + this + " is not an interface!");
      }
    if (equivalencies == null)
      { equivalencies = new Hashtable(5); }
    Vector list = (Vector) equivalencies.get(ntcp);
    if (list == null)
      { list = new Vector(); equivalencies.put(ntcp, list); }
    list.addElement(target);
  }

  // Add a class to the list of classes affected by a particular
  // method. This makes senses only for interfaces, so perform a
  // check for that too.

  private void addAffected(ClassInfo target, NameTypeCP n)
  {
    if ((permissions() & ACC_INTERFACE) == 0)
      {
        throw new RuntimeException
          ("ASSERT failed: "+target+" tried to add " + n + " as interface dependency on "+this);
      }
    if (affectsTable == null)
      { affectsTable = new Hashtable(10); }
    Hashtable h = (Hashtable)(affectsTable.get(n));
    if (h == null)
      { affectsTable.put(n, h = new Hashtable(20)); }
    //    env.stats.verboseMessage("Added " + target + " to " + this + " over " + n);
    h.put(target, target);
  }

  private String[] getFullDependencies()
  {
    Vector retV = new Vector();
    for (int i=0; i<oldCps.length; i++)
      {
        if (oldCps[i] instanceof ClassCP)
          {
            String cname = ((ClassCP)oldCps[i]).originalName();
                                // check for "array" classes
            if (cname.charAt(0) == '[')
              {
                if (cname.indexOf('L') > 0)
                  {
                    cname = cname.substring(cname.indexOf('L')+1,
                                            cname.indexOf(';'));
                  }
                else
                  { continue; }
              }
            retV.addElement(cname);
          }
      }

    String[] ret = new String[retV.size()];
    retV.copyInto(ret);
    return ret;
  }

  // Return only superclass and implemented classes of this 
  // class
  private String[] getMinimalDependencies()
  {
    String ret[];
    int x =0;
    if (super_class != null)
      {
        ret = new String[1 + interfaces.length];
        ret[0] = super_class.originalName();
        x = 1;
      }
    else
      {
        ret = new String[interfaces.length];
      }
    for (int i=0; i<interfaces.length; i++)
      {
        ret[x + i] = interfaces[i].originalName();
      }
    return ret;
  }
                                // This checks if the current class
                                // is an inner class, and sets associated
                                // variables
  private void checkForInner(InnerClassesAttr att)
  {
    for (int i=0; i<att.nameS.length; i++)
      {
        if (att.inner[i] == this_class)
          {
            isInner = true;
            parentScopeName = att.outer[i].Sname;
            oldInnerName = att.nameS[i].val;
            int start = parentScopeName.length();
            int end = (oldInnerName!=null)?oldInnerName.length():0;
            end = Sthis_class.length() - end;
            innerSeparator = Sthis_class.substring(start, end);
            /*
            System.out.println("Inner Class");
            System.out.println("Parent name " + parentScopeName);
            System.out.println("Inner separator " + innerSeparator);
            System.out.println("Inner name " + oldInnerName);
            */
            break;
          }
      }
  }
}
